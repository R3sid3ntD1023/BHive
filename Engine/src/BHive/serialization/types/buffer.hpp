#pragma once

#include "core/Buffer.h"
#include "serialization/Helpers.h"

namespace BHive
{
	template<typename TArchive, typename T>
	void Serialize(TArchive& ar, const TBuffer<T>& buffer)
	{
		ar(buffer.mSize);
		ar(BinaryData<T>(buffer.mData, buffer.mSize));
	}

	template <typename TArchive, typename T>
	void Deserialize(TArchive &ar, TBuffer<T> &buffer)
	{
		size_t size = 0;
		ar(size);

		if (size)
		{
			buffer.Allocate(size);
			ar(BinaryData<T>(buffer.mData, buffer.mSize));
		}
	}
}