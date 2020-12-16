#pragma once

#include <Shared/Types.h>

namespace asteroids
{
	class PhysicsComponent
	{

	public:

		float32 positionX = static_cast<float32>(rand() % 1000);
		float32 positionY = static_cast<float32>(rand() % 1000);
		u16 rotation = 0;
		float32 velocityX = 0.f;
		float32 velocityY = 0.f;
	};
}