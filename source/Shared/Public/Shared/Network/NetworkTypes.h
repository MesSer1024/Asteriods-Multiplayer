#pragma once

#include <Shared/Types.h>
#include <Shared/Utils.h>
#include <Shared/Debugger.h>
#include <memory>

namespace asteroids
{
	constexpr u16 	c_port = 9999;
	constexpr u32 	c_socket_buffer_size = 1024;
	constexpr u16	c_max_clients = 32;
	
	enum Client_Message : u8
	{
		Join,		// tell server we're new here
		Leave,		// tell server we're leaving
		Input 		// tell server our user input
	};

	enum Server_Message : u8
	{
		Join_Result,// tell client they're accepted/rejected
		State 		// tell client game state
	};

	struct NetworkPacket
	{
		NetworkPacket(u32 bitsUsed)
			: buffer(new u8[bitsUsed])
			, bitsUsed(bitsUsed)
		{}

		std::unique_ptr<u8> buffer;
		u32 bitsUsed;
	};

	namespace net
	{
		struct NetworkIP
		{
			NetworkIP() 
				: address{}
				, port{} 
			{
			}

			NetworkIP(u8 a, u8 b, u8 c, u8 d, u16 port)
				: address((a << 24) | (b << 16) | (c << 8) | d)
				, port(port)
			{
			}

			u32 address;
			u16 port;

			bool operator==(const NetworkIP& other) const { return address == other.address && port == other.port; }
		};

		struct Socket
		{
			SOCKET handle{};

			operator bool() const { return handle != SOCKET{}; }
		};


		static bool init()
		{
			WORD winsock_version = 0x202;
			WSADATA winsock_data;
			if (WSAStartup(winsock_version, &winsock_data))
			{
				log_warning("WSAStartup failed: %d\n", WSAGetLastError());
				return false;
			}

			return true;
		}

		static Socket CreateSocket()
		{
			int address_family = AF_INET;
			int type = SOCK_DGRAM;
			int protocol = IPPROTO_UDP;
			SOCKET sock = socket(address_family, type, protocol);

			if (sock == INVALID_SOCKET)
			{
				log_warning("outSocket failed: %d\n", WSAGetLastError());
				return Socket{};
			}

			// put socket in non-blocking mode
			u_long enabled = 1;
			int result = ioctlsocket(sock, FIONBIO, &enabled);
			if (result == SOCKET_ERROR)
			{
				log_warning("ioctlsocket failed: %d\n", WSAGetLastError());
				return Socket{};
			}

			// create socket
			Socket out_socket;
			out_socket.handle = sock;
			return out_socket;
		}

		static bool BindSocket(Socket& outSocket, NetworkIP local_endpoint)
		{
			auto endpointToSocket = [](const NetworkIP& endpoint)->SOCKADDR_IN {
				SOCKADDR_IN sockaddr_in;
				sockaddr_in.sin_family = AF_INET;
				sockaddr_in.sin_addr.s_addr = htonl(endpoint.address);
				sockaddr_in.sin_port = htons(endpoint.port);
				return sockaddr_in;
			};

			// bind socket
			SOCKADDR_IN localSocket = endpointToSocket(local_endpoint);
			if (bind(outSocket.handle, (SOCKADDR*)&localSocket, sizeof(localSocket)) == SOCKET_ERROR)
			{
				log_warning("bind failed: %d\n", WSAGetLastError());
				outSocket.handle = 0;
				return false;
			}

			return true;
		}

		static bool SendSocket(const Socket& socket, const NetworkPacket& packet, NetworkIP endpoint)
		{
			SOCKADDR_IN server_address;
			server_address.sin_family = AF_INET;
			server_address.sin_addr.S_un.S_addr = htonl(endpoint.address);
			server_address.sin_port = htons(endpoint.port);
			int server_address_size = sizeof(server_address);

			//printf("Sent to client: b0: %d, b1: %d, b2: %d\n", packet[0], packet[1], packet[2]);

			if (sendto(socket.handle, reinterpret_cast<const char*>(packet.buffer.get()), packet.bitsUsed, 0, (SOCKADDR*)&server_address, server_address_size) == SOCKET_ERROR)
			{
				log_warning("sendto failed: %d\n", WSAGetLastError());
				return false;
			}

			return true;
		}

		static bool ReceiveSocket(const Socket& socket, NetworkPacket& packet, NetworkIP& outFrom, u32& outBytesReceived)
		{
			int flags = 0;
			SOCKADDR_IN from;
			int from_size = sizeof(from);
			int bytesReceived = recvfrom(socket.handle, reinterpret_cast<char*>(packet.buffer.get()), packet.bitsUsed, flags, (SOCKADDR*)&from, &from_size);

			if (bytesReceived == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
				{
					log_warning("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d\n", error);
				}

				return false;
			}

			outFrom = {};
			outFrom.address = ntohl(from.sin_addr.S_un.S_addr);
			outFrom.port = ntohs(from.sin_port);

			outBytesReceived = bytesReceived;

			return true;
		}
	}
}