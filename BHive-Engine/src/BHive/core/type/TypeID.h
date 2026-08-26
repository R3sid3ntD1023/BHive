#pragma once

#include <stdint.h>

namespace BHive
{
	inline uint32_t NextTypeID()
	{
		static uint32_t counter = 0;
		return counter++;
	}

	template <typename T>
	struct TypeID
	{
		static uint32_t value;
	};

	template <typename T>
	uint32_t TypeID<T>::value = NextTypeID();

} // namespace BHive
