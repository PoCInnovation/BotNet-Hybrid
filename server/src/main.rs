use std::{net::{TcpListener, TcpStream}, io::{BufReader, BufRead, Write}, thread};

fn main() {
    let listener = TcpListener::bind("192.168.0.12:9570").unwrap();
    let mut ips = Vec::new();

    for stream in listener.incoming() {
        let stream = stream.unwrap();

        println!("Connection established");
        let current_ip = stream.peer_addr().unwrap();
        ips.push(current_ip);
        println!("{}", current_ip);
        thread::spawn(|| {
            handle_connection(stream);
        });
    }
}

fn handle_connection(mut stream: TcpStream) {
    let buf_reader = BufReader::new(&stream);
    let http_request: Vec<_> = buf_reader
        .lines()
        .map(|result| result.unwrap())
        .take_while(|line| !line.is_empty())
        .collect();

    println!("Request: {:?}", http_request);
    let response = "Test coucou\n";

    stream.write_all(response.as_bytes()).unwrap();
}
