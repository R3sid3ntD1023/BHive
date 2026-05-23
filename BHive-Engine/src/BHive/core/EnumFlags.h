#pragma once



#define ENUM_FLAGS(TEnum)														\
	inline TEnum operator|(TEnum lhs, TEnum rhs)											\
	{																				\
	using underlying_type = std::underlying_type_t<TEnum>;							\
	return (TEnum)((underlying_type)lhs | (underlying_type)rhs);					\
	}																				\
	inline TEnum operator&(TEnum lhs, TEnum rhs)											\
	{																				\
	using underlying_type = std::underlying_type_t<TEnum>;							\
	return (TEnum)((underlying_type)lhs & (underlying_type)rhs);					\
	}																				\
	inline TEnum operator^(TEnum lhs, TEnum rhs)											\
	{																				\
	using underlying_type = std::underlying_type_t<TEnum>;							\
	return (TEnum)((underlying_type)lhs & (underlying_type)rhs);					\
	}																				\
	inline TEnum operator~(TEnum lhs)														\
	{																				\
	using underlying_type = std::underlying_type_t<TEnum>;							\
	return (TEnum)(~(underlying_type)lhs);											\
	}																				\
	inline TEnum& operator|=(TEnum& lhs, TEnum rhs)											\
	{																				\
	return lhs = lhs | rhs;															\
	}																				\
	inline TEnum& operator&=(TEnum& lhs, TEnum rhs)											\
	{																				\
	return lhs = lhs & rhs;															\
	}																				\
	inline TEnum& operator^=(TEnum lhs, TEnum& rhs)											\
	{																				\
	return lhs = lhs ^ rhs;															\
	}

	
	#define ENABLE_BITMASK_OPERATORS(x)                                                 \
	inline x operator|(x a, x b)                                                    \
	{                                                                               \
		return static_cast<x>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b)); \
	}                                                                               \
	inline x operator&(x a, x b)                                                    \
	{                                                                               \
		return static_cast<x>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b)); \
	}                                                                               \
	inline x operator^(x a, x b)                                                    \
	{                                                                               \
		return static_cast<x>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b)); \
	}                                                                               \
	inline x operator~(x a)                                                         \
	{                                                                               \
		return static_cast<x>(~static_cast<uint32_t>(a));                           \
	}                                                                               \
	inline x &operator|=(x &a, x b)                                                 \
	{                                                                               \
		a = a | b;                                                                  \
		return a;                                                                   \
	}                                                                               \
	inline x &operator&=(x &a, x b)                                                 \
	{                                                                               \
		a = a & b;                                                                  \
		return a;                                                                   \
	}                                                                               \
	inline x &operator^=(x &a, x b)                                                 \
	{                                                                               \
		a = a ^ b;                                                                  \
		return a;                                                                   \
	}