#pragma once

#include "core/Buffer.h"
#include <cereal/cereal.hpp>

namespace BHive
{
	template <typename A, typename T>
	inline void CEREAL_SAVE_FUNCTION_NAME(A &ar, const TBuffer<T> &buffer)
	{
		auto size = buffer.GetSize();
		ar(size);
		if (size)
		{
			ar(cereal::binary_data(buffer.GetData(), buffer.GetSize() * sizeof(T)));
		}
	}

	template <typename A, typename T>
	inline void CEREAL_LOAD_FUNCTION_NAME(A &ar, TBuffer<T> &buffer)
	{
		size_t size = 0;
		ar(size);

		if (size)
		{
			buffer.Allocate(size);
			ar(cereal::binary_data(buffer.GetData(), size * sizeof(T)));
		}
	}

} // namespace BHive