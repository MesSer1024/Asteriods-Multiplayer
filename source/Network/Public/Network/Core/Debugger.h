#pragma once

#include <Network/Core/Project.h>

namespace dud
{
	static void writeLog(const char* fmt)
	{
		printf(fmt);
	}

	static void log_warning(const char* fmt, int arg)
	{
		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), fmt, arg);
		OutputDebugStringA(buffer);
	}
}
