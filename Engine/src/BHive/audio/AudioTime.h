#pragma once

#include "core/Core.h"
#include "reflection/Reflection.h"

namespace BHive
{
	struct BHIVE AudioTime
	{
		uint32_t mHours;
		uint32_t mMinutes;
		uint32_t mSeconds;

		AudioTime(float seconds)
		{
			mHours = (uint32_t)(seconds / 3600);
			seconds = fmodf(seconds, 3600);
			mMinutes = (uint32_t)(seconds / 60);
			seconds = fmodf(seconds, 60);
			mSeconds = (uint32_t)seconds;
		}

		AudioTime(uint32_t h, uint32_t m, uint32_t s)
			: mHours(h), mMinutes(m), mSeconds(s)
		{
		}

		std::string to_string() const
		{
			return std::format("{:02}:{:02}:{:02}", mHours, mMinutes, mSeconds);
		}

		REFLECTABLE()
	};

	REFLECT(AudioTime)
	{
		BEGIN_REFLECT(AudioTime)
		REFLECT_PROPERTY_READ_ONLY("Hours", mHours)
		REFLECT_PROPERTY_READ_ONLY("Minutes", mMinutes)
		REFLECT_PROPERTY_READ_ONLY("Seconds", mSeconds)
		REFLECT_PROPERTY_READ_ONLY("Time", to_string);
	}

	
	
}