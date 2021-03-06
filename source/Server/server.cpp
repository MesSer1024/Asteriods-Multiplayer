/*
Supported message types and what information is expected

Client_Message: Join
int8: Message type

Server_Message: Join_Result
int8: Message type
int8: 1 or 0 (success or fail)
int8: ID to be used when sending other message from client

Client_Message: leave
int8: Message type
int8: ID

Client_Message: Input
int8: Message type
int8: ID
int8: Input (up, down, right and/or left)

Server_Message: State
int8: Message type
For each client
int8: ClientId
float32: Position X
float32: Position Y
int16: Rotation

*/

/*
TODO: 

- Implement max speed
- Implement
- Fake lag
- Prediction and correction support for objects (and on client)
- Make this an ECS

- Server authorative asteroids
- Collission
- Bullets
- Health


*/

#include "server_net.cpp"
#include "PhysicsComponent.h"
#include "PhysicsSystem.h"

#include <Network/CoreIncludes.h>
#include <Shared/GameplayConcepts.h>
#include <Shared/Utils.h>
#include <Network/Game/PlayerInput.h>

void main()
{
	using namespace dud;

	writeLog("Server started\n");

	if (!net::init())
	{
		writeLog("net::init failed\n");
		return;
	}

	
	net::NetworkIP local_endpoint = {};
	local_endpoint.address = INADDR_ANY;
	local_endpoint.port = c_port;

	net::Socket socket = net::CreateSocket();
	bool binded = socket ? net::BindSocket(socket, local_endpoint) : false;
	if (!binded)
	{
		writeLog("Failed to bind or create socket \n");
		return;
	}

	Timing_Info timing_info = timing_info_create();

	NetworkPacket receivedPacket(c_socket_buffer_size);
	u8* inBuffer = receivedPacket.buffer.get();

	NetworkPacket outgoingPacket(c_socket_buffer_size);
	u8* outBuffer = outgoingPacket.buffer.get();

	net::NetworkIP client_endpoints[c_max_clients];
	float32 time_since_heard_from_clients[c_max_clients] = {};
	PhysicsComponent client_objects[c_max_clients];
	InputState client_inputs[c_max_clients];

	for (u16 i = 0; i < c_max_clients; ++i)
	{
		client_endpoints[i] = {};
	}

	bool is_running = true;
	while (is_running)
	{
		LARGE_INTEGER timestampFrameStart;
		QueryPerformanceCounter(&timestampFrameStart);

		// read all available packets
		net::NetworkIP from;
		u32 bytes_received;
		while (net::ReceiveSocket(socket, receivedPacket, from, bytes_received))
		{

			//Check type of message, first byte describes that
			switch (inBuffer[0])
			{
			case Client_Message::Join:
			{
				writeLog("Client joining...");

				//Check if there are any empty slots
				u8 slot = (u8)-1;
				for (u8 i = 0; i < c_max_clients; ++i)
				{
					if (client_endpoints[i].address == 0)
					{
						slot = i;
						break;
					}
				}

				outBuffer[0] = (s8)Server_Message::Join_Result;

				//If available slot assign and reply back
				if (slot != (u8)-1)
				{
					log_warning("client slot = %hu\n", slot);
					ConnectionId connectionId;
					connectionId.setIndex(slot);

					outBuffer[0] = Server_Message::Join_Result;

					//Set second byte to 1 for indicating success 
					outBuffer[1] = 1;

					//Set third byte to the client ID, so that the client can use that when sending input message
					outBuffer[2] = static_cast<u8>(connectionId.getIndex());

					if (net::SendSocket(socket, outgoingPacket, from))
					{
						PhysicsComponent newObject;

						//Set random rotation and add velocity, only so that I can test with many clients!
						newObject.rotation = rand() % 360;
						newObject.velocityX += static_cast<float32>(sin(((float32)newObject.rotation / 180 * M_PI)));
						newObject.velocityY += static_cast<float32>(-cos(((float32)newObject.rotation / 180 * M_PI)));


						client_endpoints[connectionId] = from;
						time_since_heard_from_clients[connectionId] = 0.0f;
						client_objects[connectionId] = newObject;
						client_inputs[connectionId].reset();
					}
					else
					{
						log_warning("sendto failed: %d\n", WSAGetLastError());
					}
				}
				else
				{
					writeLog("could not find a slot for player\n");
					outBuffer[1] = 0;

					if (!net::SendSocket(socket, outgoingPacket, from))
					{
						log_warning("sendto failed: %d\n", WSAGetLastError());
					}
				}
			}
			break;

			case Client_Message::Leave:
			{
				ConnectionId connectionId;
				connectionId.setIndex(inBuffer[1]);

				if (client_endpoints[connectionId] == from)
				{
					client_endpoints[connectionId] = {};
					printf("Client_Message::Leave from %hu(%u:%hu)\n",
						connectionId.getIndex(), from.address, from.port);
				}
				else
				{
					printf("Client_Message::Leave from %hu(%u:%hu), expected (%u:%hu)",
						connectionId.getIndex(), from.address, from.port,
						client_endpoints[connectionId].address, client_endpoints[connectionId].port);
				}
			}
			break;

			case Client_Message::Input:
			{
				ConnectionId connectionId;
				connectionId.setIndex(inBuffer[1]);

				//Check so that the ID matches where it comes from
				if (client_endpoints[connectionId] == from)
				{
					u8 input = inBuffer[2];

					client_inputs[connectionId].pressedConcepts = input;

					time_since_heard_from_clients[connectionId] = 0.0f;

				}
				else
				{
					printf("Client address not matching i_Message::Input discarded, was from %u:%hu but expected %u:%hu\n", from.address, from.port, client_endpoints[connectionId].address, client_endpoints[connectionId].port);
				}
			}
			break;
			}
		}

		// process input and update state
		for (u8 i = 0; i < c_max_clients; ++i)
		{

			if (client_endpoints[i].address)
			{
				if (client_inputs[i].isPressed(InputConcept::Thrust))
				{
					client_objects[i].velocityX += static_cast<float32>(sin(((float32)client_objects[i].rotation / 180 * M_PI))) / 10;
					client_objects[i].velocityY += static_cast<float32>(-cos(((float32)client_objects[i].rotation / 180 * M_PI))) / 10;
				}
				if (client_inputs[i].isPressed(InputConcept::RotateLeft))
				{
					//Ensure correct degree
					if (client_objects[i].rotation == 0)
						client_objects[i].rotation = 360 - c_turn_speed;
					else
						client_objects[i].rotation -= c_turn_speed;

				}
				if (client_inputs[i].isPressed(InputConcept::RotateRight))
				{
					//Ensure correct degrees
					if (client_objects[i].rotation == 360)
						client_objects[i].rotation = c_turn_speed;
					else
						client_objects[i].rotation += c_turn_speed;

				}

				time_since_heard_from_clients[i] += c_seconds_per_tick;
				if (time_since_heard_from_clients[i] > c_client_timeout)
				{
					log_warning("client %hu timed out\n", i);
					client_endpoints[i] = {};
				}


				//Updatet this objects position
				updateServerObjectPosition(client_objects[i]);

			}
		}

		// create state packet (1 byte)
		outBuffer[0] = (s8)Server_Message::State;
		s32 bytes_written = 1;

		for (u8 i = 0; i < c_max_clients; ++i)
		{
			if (client_endpoints[i].address)
			{

				//Set client ID (1 byte)
				memcpy(&outBuffer[bytes_written], &i, sizeof(i));
				bytes_written += sizeof(i);

				//Set client X position (4 byte)
				memcpy(&outBuffer[bytes_written], &client_objects[i].positionX, sizeof(client_objects[i].positionX));
				bytes_written += sizeof(client_objects[i].positionX);

				//Set client Y position (4 byte)
				memcpy(&outBuffer[bytes_written], &client_objects[i].positionY, sizeof(client_objects[i].positionY));
				bytes_written += sizeof(client_objects[i].positionY);

				//Set client rotation (2 byte)
				memcpy(&outBuffer[bytes_written], &client_objects[i].rotation, sizeof(client_objects[i].rotation));
				bytes_written += sizeof(client_objects[i].rotation);

			}

			//Reset client inputs once per frame
			client_inputs[i] = {};

		}

		// send back to clients
		for (u8 i = 0; i < c_max_clients; ++i)
		{
			if (client_endpoints[i].address)
			{
				NetworkPacket data(bytes_written);
				memcpy(data.buffer.get(), outBuffer, bytes_written);
				if (!net::SendSocket(socket, data, client_endpoints[i]))
				{
					log_warning("sendto failed: %d\n", WSAGetLastError());
				}
			}
		}

		wait_for_tick_end(timestampFrameStart, &timing_info);
	}
}
