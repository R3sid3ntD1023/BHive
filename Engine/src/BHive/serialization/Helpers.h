#pragma once

#include <type_traits>

namespace BHive
{
	template<typename T>
	struct BinaryData
	{

		BinaryData(T* data, size_t size)
			: mData(data),
			  mSize(size)
		{}

		T* mData = nullptr;
		size_t mSize = 0;
	};
}