#pragma once

#include <stdint.h>

namespace BHive
{
	using JoyStickAxisCode = uint8_t;
	using JoyStickButtonCode = uint8_t;

	namespace JoyStickAxis
	{
		enum : JoyStickAxisCode
		{
			LEFT	        ,
			RIGHT			,
			TRIGGERS		,
			LAST			= TRIGGERS,
		};
	}

	namespace JoyStickButton
	{
		enum : JoyStickButtonCode
		{
			A               = 0 ,
			B               = 1 ,
			X               = 2 ,
			Y               = 3 ,
			LEFT_BUMPER     = 4 ,
			RIGHT_BUMPER    = 5 ,
			BACK            = 6 ,
			START           = 7 ,
			GUIDE           = 8 ,
			LEFT_THUMB      = 9 ,
			RIGHT_THUMB     = 10,
			DPAD_UP         = 11,
			DPAD_RIGHT      = 12,
			DPAD_DOWN       = 13,
			DPAD_LEFT       = 14,
			LAST            = DPAD_LEFT,
			
			CROSS			= A,
			CIRCLE			= B,
			SQUARE			= X,
			TRIANGLE		= Y,
		};
	}
}