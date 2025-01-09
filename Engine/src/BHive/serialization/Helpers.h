#pragma once

#include <type_traits>

namespace BHive
{
	template<typename T>
	struct BinaryData
	{
		using PT = typename std::conditional_t<
			std::is_const_v<typename std::remove_pointer_t<typename std::remove_reference_t<T>>>,
			const void *, void *>;

		BinaryData(T&& data, size_t size)
			: mData(std::forward<T>(data)),
			  mSize(size)
		{}

		PT mData = nullptr;
		size_t mSize = 0;
	};
}