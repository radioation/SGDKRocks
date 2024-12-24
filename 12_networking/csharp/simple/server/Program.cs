
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace TcpEchoServer
{
	class Program
	{
		static async Task Main(string[] args)
		{
			if (args.Length != 1)
			{
				Console.WriteLine("Usage: TcpEchoServer <server_ip>");
				return;
			}
			var serverIp = args[0];
			var listener = new TcpListener(IPAddress.Parse(serverIp), 8080);
			listener.Start();

			while (true)
			{
				var client = await listener.AcceptTcpClientAsync();
				Task.Run(() => HandleClient(client));
			}
		}

		static async Task HandleClient(TcpClient client)
		{
			try
			{
				using (var stream = client.GetStream())
				{
					byte[] buffer = new byte[1024];
					int bytesRead;

					while ((bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length)) > 0)
					{
						var str = System.Text.Encoding.Default.GetString(buffer);
						Console.WriteLine( str );
						await stream.WriteAsync(buffer, 0, bytesRead);
					}
				}
			}
			catch (Exception e)
			{
				Console.Error.WriteLine("Error handling client: {0}", e.Message);
			}
			finally
			{
				client.Close();
			}
		}
	}
}
