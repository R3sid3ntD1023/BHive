#pragma once

#include <stdint.h>

namespace BHive
{
	using ModCode = uint8_t;

	namespace Mod
	{
		enum : ModCode
		{
			Shift = 1 << 0,
			Control = 1 << 1,
			Alt = 1 << 2,
			Super = 1 << 3,
			Caps_lock = 1 << 4,
			Num_lock = 1 << 5
		};
	}
} // namespace BHive