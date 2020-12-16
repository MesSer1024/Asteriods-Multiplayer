#pragma once

#include <Shared/Network/NetworkTypes.h>

namespace dud
{
	namespace net
	{


		static float32 time_since(LARGE_INTEGER t, LARGE_INTEGER frequency)
		{
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);

			return float32(now.QuadPart - t.QuadPart) / float32(frequency.QuadPart);
		}

	} // namespace Net
}