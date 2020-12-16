#pragma once

#include <Shared/GameplayConcepts.h>
#include <Shared/Utils.h>

namespace dud
{

	float32 time_since(LARGE_INTEGER t, LARGE_INTEGER frequency)
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);

		return (float32)(now.QuadPart - t.QuadPart) / (float32)frequency.QuadPart;
	}

	Timing_Info timing_info_create()
	{
		Timing_Info timing_info = {};

		UINT sleep_granularity_ms = 1;
		timing_info.sleep_granularity_was_set = timeBeginPeriod(sleep_granularity_ms) == TIMERR_NOERROR;

		QueryPerformanceFrequency(&timing_info.clock_frequency);

		return timing_info;
	}

	void wait_for_tick_end(LARGE_INTEGER tick_start_time, Timing_Info* timing_info)
	{
		float32 time_taken_s = time_since(tick_start_time, timing_info->clock_frequency);

		while (time_taken_s < c_seconds_per_tick)
		{
			if (timing_info->sleep_granularity_was_set)
			{
				DWORD time_to_wait_ms = (DWORD)((c_seconds_per_tick - time_taken_s) * 1000);
				if (time_to_wait_ms > 0)
				{
					Sleep(time_to_wait_ms);
				}
			}

			time_taken_s = time_since(tick_start_time, timing_info->clock_frequency);
		}
	}
}