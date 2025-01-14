#pragma once

#include <cereal/cereal.hpp>
#include "core/Buffer.h"

namespace BHive
{
	template <typename A, typename T>
	inline void CEREAL_SAVE_FUNCTION_NAME(A &ar, const TBuffer<T> &buffer)
	{
		ar(buffer.mSize);
		ar(cereal::binary_data(const_cast<T*>(buffer.mData), buffer.mSize * sizeof(T)));
	}

	template <typename A, typename T>
	inline void CEREAL_LOAD_FUNCTION_NAME(A &ar, TBuffer<T> &buffer)
	{
		size_t size = 0;
		ar(size);
		buffer.Allocate(size);
		ar(cereal::binary_data(buffer.mData, buffer.mSize * sizeof(T)));
	}

	template <typename A>
	inline void CEREAL_SAVE_FUNCTION_NAME(A &ar, const Buffer &buffer)
	{
		ar(buffer.mSize);
		ar(cereal::binary_data(const_cast<uint8_t *>(buffer.mData), buffer.mSize * sizeof(uint8_t)));
	}

	template <typename A>
	inline void CEREAL_LOAD_FUNCTION_NAME(A &ar, Buffer &buffer)
	{
		size_t size = 0;
		ar(size);
		buffer.Allocate(size);
		ar(cereal::binary_data(buffer.mData, buffer.mSize * sizeof(uint8_t)));
	}

} // namespace BHive