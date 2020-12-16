#pragma once

#include <Network/CoreIncludes.h>

namespace dud
{
	float32 time_since(LARGE_INTEGER t, LARGE_INTEGER frequency);

	Timing_Info timing_info_create();

	void wait_for_tick_end(LARGE_INTEGER tick_start_time, Timing_Info* timing_info);
}
