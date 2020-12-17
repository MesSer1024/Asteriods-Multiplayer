#pragma once

#include <Network/CoreIncludes.h>

namespace dud
{
	constexpr u32 c_ticks_per_second = 60;
	constexpr float32 c_seconds_per_tick = 1.0f / (float32)c_ticks_per_second;

	constexpr u16 c_turn_speed = 5;	// how fast player turns
	constexpr float32 c_max_speed = 50.f; //TODO: Implement so that we have a max speed
	constexpr float32 c_client_timeout = 60.f;
}
