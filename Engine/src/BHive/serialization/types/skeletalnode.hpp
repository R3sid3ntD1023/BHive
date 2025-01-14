#pragma once

#include <cereal/cereal.hpp>
#include "mesh/SkeletalNode.h"

namespace BHive
{
	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, SkeletalNode &obj)
	{
		ar(cereal::make_nvp("Name", obj.mName), cereal::make_nvp("Transformation", obj.mTransformation),
		   cereal::make_nvp("Children", obj.mChildren));
	}
} // namespace BHive