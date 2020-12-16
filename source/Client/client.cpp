#include <SFML/Graphics.hpp>
#include <iostream>


#include <Shared/Utils.h>
#include <Shared/Network/NetworkTypes.h>
#include "client_object_component.cpp"
#include <Shared/Network/Serialization.h>

/* IMPROVEMENTS
- Being able to press multiple keys simulatnious
- Max speed
- Window management
- Move object from one side to another
- ECS?
- Asteroids simulation on server.

*/

namespace asteroids
{

    struct object {
        sf::CircleShape mesh;
        sf::Vector2f velocity;
    };

    struct input {
        bool thrust, rotateLeft, rotateRight;
    };
    static input user_input;
}

int main()
{
    using namespace asteroids;

    //Package drop variables
    u8 drop_counter = 0;
    bool loss_sequence_running = false;
    u8 packages_lost = rand() % 20;

    //Variables
    int speed = 5;
    sf::Vector2f midScreen(500, 500);
    sf::Vector2f velocity(0.f, 0.f);
    sf::Vector2f asteroid_velocity(1.f, 1.f);

    u8 clientId = 0xFF;

    //Init window
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Massive Asteroids Multiplayer");
    window.setFramerateLimit(60);

    //TODO: Implement visual thurst that only lives on client

    ClientObject allShips[c_max_clients];

    //Struct asteriod
    struct object asteroid;
    asteroid.mesh.setRadius(50);
    asteroid.mesh.setFillColor(sf::Color::Red);
    asteroid.mesh.setPosition(sf::Vector2f(100, 50));

    //Tick initialization (TODO: Make sure it's same as server)
    UINT sleep_granularity_ms = 1;
    bool sleep_granularity_was_set = timeBeginPeriod(sleep_granularity_ms) == TIMERR_NOERROR;
    const u32	TICKS_PER_SECOND = 60;
    const float32	SECONDS_PER_TICK = 1.0f / float32(TICKS_PER_SECOND);

    LARGE_INTEGER clock_frequency;
    QueryPerformanceFrequency(&clock_frequency);

    //Init socket stuffs
    if (!net::init())
    {
        printf("net::init failed\n");
        return false;
    }

    net::Socket socket;

    if (!net::CreateSocket(&socket))
    {
        printf("CreateSocket failed\n");
        return false;
    }


    //Socket stuffs :-)
    NetworkPacket sharedPacket(c_socket_buffer_size);
    u8* sharedBuffer = sharedPacket.buffer.get();

    net::NetworkIP server_endpoint(127, 0, 0, 1, c_port);
    net::NetworkIP from;
    u32 bytes_received;

    /*Debug, join automatically!*/
    sharedBuffer[0] = (s8)Client_Message::Join;

    //TODO, only allow one ID per client, or perhaps from server to fix that!
    if (net::SendSocket(socket, sharedPacket, server_endpoint))
    {
        std::cout << "Joining server\n";
    }


    while (window.isOpen())
    {
        //Start timer
        LARGE_INTEGER tick_start_time;
        QueryPerformanceCounter(&tick_start_time);

        sf::Event event;

        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:

                switch (event.key.code)
                {

                    //Increase thrust
                case sf::Keyboard::W:
                    user_input.thrust = true;
                    break;

                    //Rotate right
                case sf::Keyboard::D:
                    user_input.rotateRight = true;

                    break;

                    //Rotate left
                case sf::Keyboard::A:
                    user_input.rotateLeft = true;

                    break;

                    //Join server
                case sf::Keyboard::J:

                    //This is moved as I want the clients to join right away instead of waiting for J to be pushed. For testing purposes

                    /*sendBuffer[0] = (int8)Client_Message::Join;

                    //TODO, only allow one ID per client, or perhaps from server to fix that!
                    if (net::socket_send(&sock, sendBuffer, c_socket_buffer_size, &server_endpoint))
                    {
                        std::cout << "Joining server\n";
                    }*/

                    break;
                }



                break;
            }
        }//End windows poll event



        //Check for responses
        while (net::ReceiveSocket(socket, sharedPacket, from, bytes_received))
        {

            u8* buffer_iter = sharedBuffer;
            u8 message_type;


            switch ((Server_Message)sharedBuffer[0])
            {

            case Server_Message::Join_Result:

                u8 success;
                //Deserialize first uint8 of message (Message type)
                deserialize_u8(&buffer_iter, &message_type);


                //Deserilize second uint8 of message (Success or not)
                deserialize_u8(&buffer_iter, &success);

                //If join was a success
                if (success)
                {
                    //Deserilize third uint8 of message (ID)
                    deserialize_u8(&buffer_iter, &clientId);
                }

                printf("Joined server with ClientID = %d\n", clientId);

                allShips[clientId].mesh.setFillColor(sf::Color::Blue);

                break;

            case Server_Message::State:

#ifdef PACKAGE_LOSS
                //Randomize when package loss starts
                if (!loss_sequence_running && ((rand() % 20) == 0))
                {
                    loss_sequence_running = true;
                }
                //Randomize how many packages are lost every time
                if (drop_counter > packages_lost)
                {
                    loss_sequence_running = false;
                    drop_counter = 0;
                    packages_lost = rand() % 20;
                }
                //Count packages lost
                if (loss_sequence_running) {
                    drop_counter++;
                }
                //Do magic when we actually got a package through :-)
                else
                {
#endif
                    u8 bufferClientId;
                    float32 bufferPositionX = 0.f;
                    float32 bufferPositionY = 0.f;
                    u16 bufferRotation = 0;
                    u32 bytes_read = 0;

                    //Get message type
                    deserialize_u8(&buffer_iter, &message_type);
                    bytes_read++;

                    while (bytes_read < bytes_received)
                    {
                        //Get client ID
                        deserialize_u8(&buffer_iter, &bufferClientId);
                        bytes_read++;

                        //Get positionX
                        deserialize_f32(&buffer_iter, &bufferPositionX);
                        bytes_read += 4;

                        //Get positionY
                        deserialize_f32(&buffer_iter, &bufferPositionY);
                        bytes_read += 4;

                        allShips[bufferClientId].setVelocity(bufferPositionX, bufferPositionY);


                        //Get rotation
                        deserialize_u16(&buffer_iter, &bufferRotation);
                        bytes_read += 2;

                        allShips[bufferClientId].mesh.setRotation(bufferRotation);
                        allShips[bufferClientId].setPosition();

                        allShips[bufferClientId].connected = true;

                    }
#ifdef PACKAGE_LOSS
                }
#endif
                break;
            }

        }

        //Send input to server, if any
        if ((clientId != 0xFF) && (user_input.thrust || user_input.rotateLeft || user_input.rotateRight))
        {
            //1 byte: Set message type
            sharedBuffer[0] = (Client_Message::Input);
            int bytes_written = 1;

            //2 byte: client ID
            sharedBuffer[1] = clientId;
            bytes_written += sizeof(clientId); //memcpy(&sendBuffer[bytes_written], &clientId, sizeof(clientId)); //Question: Why would you use memcpy?

            //3 byte: Set first bit to current thurst, second bit to rotateLeft and third bit to rotateRight
            u8 send_input = (u8)user_input.thrust | (u8)user_input.rotateLeft << 1 | (u8)user_input.rotateRight << 2;
            sharedBuffer[2] = send_input;

            //Collect all key pressed in tick and send once per tick
            if (!net::SendSocket(socket, sharedPacket, server_endpoint))
            {
                std::cout << "\nDidn't send key pressed";
            }
        }


        asteroid.mesh.move(asteroid_velocity);

        window.clear();


        for (u8 i = 0; i < c_max_clients; ++i)
        {
            if (allShips[i].connected)
            {
                window.draw(allShips[i].mesh);
            }
        }


        //window.draw(asteroid.mesh);
        window.display();


        //Wait until tick is done to continue with next tick
        float32 time_taken_s = time_since(tick_start_time, clock_frequency);

        while (time_taken_s < SECONDS_PER_TICK)
        {
            if (sleep_granularity_was_set)
            {
                DWORD time_to_wait_ms = DWORD((SECONDS_PER_TICK - time_taken_s) * 1000);
                if (time_to_wait_ms > 0)
                {
                    Sleep(time_to_wait_ms);
                }
            }

            time_taken_s = time_since(tick_start_time, clock_frequency);
        }

        user_input.thrust = NULL;
        user_input.rotateRight = NULL;
        user_input.rotateLeft = NULL;

    }

    window.close();
    return 0;
}