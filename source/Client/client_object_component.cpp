#include <SFML/Graphics.hpp>
#include <Shared/Utils.h>

namespace asteroids
{
    class ClientObject {

    public:
        sf::CircleShape mesh;
        sf::Vector2f velocity;
        bool connected = false;

        ClientObject() {

            mesh.setRadius(10.f);
            mesh.setPointCount(3);
            mesh.setFillColor(sf::Color::Yellow);
            mesh.setScale(1.f, 1.5f);
            mesh.setOrigin(10.f, 10.f);
            mesh.setPosition(500, 500);

        }
        void setVelocity(float32 x, float32 y)
        {
            velocity.x = x;
            velocity.y = y;
        }
        void setPosition()
        {
            mesh.setPosition(velocity);
        }
        void setRotation(u8 rotation)
        {
            mesh.rotate(rotation);
        }
    };
}