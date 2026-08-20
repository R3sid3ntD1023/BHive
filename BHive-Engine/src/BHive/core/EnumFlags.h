#pragma once

#define ENABLE_BITMASK_OPERATORS(TEnum)                              \
	inline TEnum operator|(TEnum lhs, TEnum rhs)                     \
	{                                                                \
		using underlying_type = std::underlying_type_t<TEnum>;       \
		return (TEnum)((underlying_type)lhs | (underlying_type)rhs); \
	}                                                                \
	inline TEnum operator&(TEnum lhs, TEnum rhs)                     \
	{                                                                \
		using underlying_type = std::underlying_type_t<TEnum>;       \
		return (TEnum)((underlying_type)lhs & (underlying_type)rhs); \
	}                                                                \
	inline TEnum operator^(TEnum lhs, TEnum rhs)                     \
	{                                                                \
		using underlying_type = std::underlying_type_t<TEnum>;       \
		return (TEnum)((underlying_type)lhs & (underlying_type)rhs); \
	}                                                                \
	inline TEnum operator~(TEnum lhs)                                \
	{                                                                \
		using underlying_type = std::underlying_type_t<TEnum>;       \
		return (TEnum)(~(underlying_type)lhs);                       \
	}                                                                \
	inline TEnum &operator|=(TEnum &lhs, TEnum rhs)                  \
	{                                                                \
		return lhs = lhs | rhs;                                      \
	}                                                                \
	inline TEnum &operator&=(TEnum &lhs, TEnum rhs)                  \
	{                                                                \
		return lhs = lhs & rhs;                                      \
	}                                                                \
	inline TEnum &operator^=(TEnum &lhs, TEnum &rhs)                 \
	{                                                                \
		return lhs = lhs ^ rhs;                                      \
	}                                                                \
	inline bool HasFlag(TEnum a, TEnum b)                            \
	{                                                                \
		using underlying_type = std::underlying_type_t<TEnum>;       \
		return static_cast<underlying_type>(a & b) != 0;             \
	}
