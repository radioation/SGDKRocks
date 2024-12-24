use tokio::net::{TcpListener, TcpStream};
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::spawn;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let listener = TcpListener::bind("10.86.22.6:8080").await?;

    loop {
        let (socket, _) = listener.accept().await?;
        spawn(handle_client(socket));
    }
}

async fn handle_client(mut socket: TcpStream) {
    let mut buf = vec![0; 1024];

    loop {
        let n = match socket.read(&mut buf).await {
            Ok(n) => n,
            Err(e) => {
                eprintln!("Error reading from socket: {}", e);
                return;
            }
        };

        if n == 0 {
            break;
        }

        if let Err(e) = socket.write_all(&buf[..n]).await {
            eprintln!("Error writing to socket: {}", e);
            return;
        }
    }
}

