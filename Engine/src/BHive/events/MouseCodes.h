#pragma once

#include <stdint.h>

namespace BHive
{
	using MouseCode = uint8_t;

	namespace Mouse
	{
		enum : MouseCode
		{
			MouseButton1        =  0,
			MouseButton2        =  1,
			MouseButton3        =  2,
			MouseButton4        =  3,
			MouseButton5        =  4,
			MouseButton6        =  5,
			MouseButton7        =  6,
			MouseButton8        =  7,
			MouseButtonLast     =  MouseButton8,
			MouseButtonLeft     =  MouseButton1,
			MouseButtonRight    =  MouseButton2,
			MouseButtonMiddle   =  MouseButton3
		};
	}
}