#pragma once

namespace BHive::utils
{
	template <typename T>
	inline size_t ComputeHash(const T &value)
	{
		return std::hash<T>{}(value);
	}

	template <typename T>
	inline size_t HashCombine(size_t &seed, const T &value)
	{
		seed ^= (ComputeHash<T>(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
		return seed;
	}

	template <typename T, typename... Rest>
	inline size_t HashCombine(size_t &seed, const T &value, const Rest &...rest)
	{
		HashCombine(seed, value);
		if constexpr (sizeof...(rest) > 0)
		{
			HashCombine(seed, rest...);
		}
		return seed;
	}

} // namespace BHive::utils