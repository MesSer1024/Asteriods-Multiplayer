#include "PhysicsComponent.h"
#include <Shared/Types.h>

namespace asteroids
{
	void updateServerObjectPosition(PhysicsComponent& object)
	{
		object.positionX += object.velocityX;
		object.positionY += object.velocityY;

		if (object.positionX < 0.0f) object.positionX = object.positionX + 1000;
		if (object.positionX >= 1000) object.positionX = object.positionX - 1000;

		if (object.positionY < 1000) object.positionY = object.positionY + 1000;
		if (object.positionY >= 1000) object.positionY = object.positionY - 1000;
	}
}
