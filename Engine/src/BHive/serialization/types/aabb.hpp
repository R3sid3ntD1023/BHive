#pragma once

#include <cereal/cereal.hpp>
#include "math/AABB.hpp"

namespace BHive
{
	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, AABB &obj)
	{
		ar(cereal::make_nvp("Min", obj.Min), cereal::make_nvp("Max", obj.Max));
	}
} // namespace BHive