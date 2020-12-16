#pragma once

#include <Shared/Project.h>

namespace asteroids
{
	using u64 = unsigned long long;
	using u32 = unsigned int;
	using u16 = unsigned short;
	using u8 = unsigned char;
	using s64 = long long;
	using s32 = int;
	using s16 = short;
	using s8 = char;
	using float32 = float;
	using float64 = double;

	struct Timing_Info
	{
		LARGE_INTEGER clock_frequency;
		bool sleep_granularity_was_set;
	};
}
