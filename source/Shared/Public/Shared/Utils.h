#pragma once

#include <Shared/Types.h>

namespace asteroids
{
#ifndef RELEASE
#define assert( x ) if( !( x ) ) { MessageBoxA( 0, #x, "Debug Assertion Failed", MB_OK ); }
#endif

	// todo(jbr) logging system
	void log_warning(const char* fmt, int arg);

	float32 time_since(LARGE_INTEGER t, LARGE_INTEGER frequency);

	Timing_Info timing_info_create();

	void wait_for_tick_end(LARGE_INTEGER tick_start_time, Timing_Info* timing_info);
}
