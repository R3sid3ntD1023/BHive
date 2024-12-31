#pragma once

#include <stdint.h>

namespace BHive
{
	using InputActionCode = uint8_t;

	namespace InputAction
	{
		enum : InputActionCode
		{
			RELEASE  =  0,
			PRESS    =  1,
			REPEAT   =  2
		};
	}

	using ModCode = uint8_t;

	namespace Mod
	{
		enum : ModCode
		{
			Shift = 0x0001,
			Control = 0x0002,
			Alt = 0x0004,
			Super = 0x0008,
			Caps_lock = 0x0010,
			Num_lock = 0x0020
		};
	}
}