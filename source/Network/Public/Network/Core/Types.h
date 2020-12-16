#pragma once

#include <Network/Core/Project.h>

namespace dud
{
	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned int;
	using u64 = unsigned long long;

	using s8 = char;
	using s16 = short;
	using s32 = int;
	using s64 = long long;

	using float32 = float;
	using float64 = double;

	using uint = u32;

	struct Timing_Info
	{
		LARGE_INTEGER clock_frequency;
		bool sleep_granularity_was_set;
	};
}
