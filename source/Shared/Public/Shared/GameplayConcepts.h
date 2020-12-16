#pragma once

#include <Shared/Types.h>

namespace asteroids
{
	constexpr u16 c_turn_speed = 5;	// how fast player turns
	constexpr float32 c_max_speed = 50.f; //TODO: Implement so that we have a max speed
	constexpr float32 c_client_timeout = 60.f;

	struct Player_Input
	{
		bool thrust, rotateLeft, rotateRight;
	};
}
