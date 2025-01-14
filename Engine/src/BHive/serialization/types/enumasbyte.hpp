#pragma once

#include <cereal/cereal.hpp>
#include "core/EnumAsByte.h"

namespace BHive
{
	template <typename A, typename T>
	inline int CEREAL_SAVE_MINIMAL_FUNCTION_NAME(const A &ar, const TEnumAsByte<T> &obj)
	{
		return obj.Get();
	}

	template <typename A, typename T>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(const A &ar, TEnumAsByte<T> &obj, const int &value)
	{
		obj = (T)value;
	}

} // namespace BHive