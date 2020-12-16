#pragma once

#include <Shared/Debugger.h>
#include <Shared/Network/NetworkTypes.h>

namespace asteroids
{
	namespace net
	{
		static SOCKADDR_IN endpointToSocket(NetworkIP* endpoint)
		{
			SOCKADDR_IN sockaddr_in;
			sockaddr_in.sin_family = AF_INET;
			sockaddr_in.sin_addr.s_addr = htonl(endpoint->address);
			sockaddr_in.sin_port = htons(endpoint->port);
			return sockaddr_in;
		}

		static bool socket_bind(Socket* socket, NetworkIP* local_endpoint)
		{
			SOCKADDR_IN localSocket = endpointToSocket(local_endpoint);
			if (bind(socket->handle, (SOCKADDR*)&localSocket, sizeof(localSocket)) == SOCKET_ERROR)
			{
				log_warning("bind failed: %d\n", WSAGetLastError());
				return false;
			}

			return true;
		}


		static float32 time_since(LARGE_INTEGER t, LARGE_INTEGER frequency)
		{
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);

			return float32(now.QuadPart - t.QuadPart) / float32(frequency.QuadPart);
		}

	} // namespace Net
}