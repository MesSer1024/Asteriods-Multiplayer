#pragma once

#include <Shared/Project.h>

namespace asteroids
{
	using uint64 = unsigned long long;
	using uint32 = unsigned int;
	using uint16 = unsigned short;
	using uint8 = unsigned char;
	using int64 = long long;
	using int32 = int;
	using int16 = short;
	using int8 = char;
	using bool32 = int;
	using float32 = float;
	using float64 = double;

	constexpr uint32	c_ticks_per_second = 60;
	constexpr float32	c_seconds_per_tick = 1.0f / (float32)c_ticks_per_second;

	struct Timing_Info
	{
		LARGE_INTEGER clock_frequency;
		bool32 sleep_granularity_was_set;
	};
}
