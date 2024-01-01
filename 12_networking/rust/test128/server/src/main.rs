use tokio::net::{TcpListener, TcpStream};
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use bytes::BytesMut;
use tokio::spawn;
use std::time::Duration;
use std::thread::sleep;


#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let listener = TcpListener::bind("10.201.138.174:8080").await?;

    loop {
        let (socket, _) = listener.accept().await?;
        spawn(handle_client(socket));
    }
}

async fn handle_client(mut socket: TcpStream) {
    let mut buf = [0; 1];
    if let Err(e) = socket.read_exact(&mut buf).await {
        eprintln!("Error reading from socket: {}", e);
        return;
    }


    let requested_length = u8::from_be_bytes(buf) as usize;
    println!(" client requested: {}", requested_length);

    let mut response = Vec::with_capacity(requested_length );


    print!(" adding to buffer: ");
    for i in 0..requested_length {
        let byte = (i & 0xff) as u8;
        print!("{} ", byte);
        response.push(byte);
    }

    println!("   about to send {} bytes: ",response.len() );
    if let Err(e) = socket.write_all(&response).await {
        eprintln!("Error writing to socket: {}", e);
        return;
    }

    sleep(Duration::from_millis(4000));  // give the genesis time to read everything before we terminate
}



