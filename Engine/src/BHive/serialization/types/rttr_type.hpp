#pragma once

#include <cereal/cereal.hpp>
#include <rttr/type.h>

namespace rttr
{

	template <typename A>
	inline std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME(const A &ar, const type &obj)
	{
		return obj.get_name().data();
	}

	template <typename A>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(const A &ar, type &obj, const std::string &value)
	{
		obj = type::get_by_name(value);
	}
} // namespace rttr