using System;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace TcpEchoClient
{
    class Program
    {
        static async Task Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("Usage: TcpEchoClient <server_ip> <message>");
                return;
            }

            var serverIp = args[0];
            var message = args[1];

            try
            {
                using (var client = new TcpClient(serverIp, 8080))
                using (var stream = client.GetStream())
                {
                    byte[] messageBytes = Encoding.ASCII.GetBytes(message);
                    await stream.WriteAsync(messageBytes, 0, messageBytes.Length);

                    byte[] buffer = new byte[1024];
                    int bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length);
                    var response = Encoding.ASCII.GetString(buffer, 0, bytesRead);

                    Console.WriteLine("Received response: {0}", response);
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("Error connecting to server: {0}", e.Message);
            }
        }
    }
}
