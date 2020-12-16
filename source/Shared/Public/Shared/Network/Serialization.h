#pragma once

#include <Shared/Types.h>

namespace asteroids
{
	static void deserialize_u8(u8** buffer, u8* u)
	{
		*u = **buffer;
		++(*buffer);
	}

	static void deserialize_u32(u8** buffer, u32* u)
	{
		memcpy(u, *buffer, sizeof(*u));
		*buffer += sizeof(*u);
	}

	static void deserialize_f32(u8** buffer, float32* f)
	{
		memcpy(f, *buffer, sizeof(*f));
		*buffer += sizeof(*f);
	}

	static void deserialize_u16(u8** buffer, u16* u)
	{
		memcpy(u, *buffer, sizeof(*u));
		*buffer += sizeof(*u);
	}
}
