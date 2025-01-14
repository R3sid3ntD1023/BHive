#pragma once

#include <cereal/cereal.hpp>
#include "mesh/Bone.h"

namespace BHive
{
	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, Bone &obj)
	{
		ar(cereal::make_nvp("Name", obj.mName), cereal::make_nvp("ID", obj.mID), cereal::make_nvp("Offset", obj.mOffset),
		   cereal::make_nvp("Parent", obj.mParent));
	}
} // namespace BHive