#pragma once

#include <Shared/Project.h>


namespace asteroids
{


	// todo(jbr) logging system
	static void log_warning(const char* fmt, int arg)
	{
		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), fmt, arg);
		OutputDebugStringA(buffer);
	}
}
