#pragma once

#include <stdint.h>

namespace BHive
{
	using EventStatusCode = uint8_t;

	namespace EventStatus
	{
		enum : EventStatusCode
		{
			RELEASE = 0,
			PRESS = 1,
			REPEAT = 2
		};
	}

} // namespace BHive