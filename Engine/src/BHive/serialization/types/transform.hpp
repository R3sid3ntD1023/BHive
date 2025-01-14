#pragma once

#include <cereal/cereal.hpp>
#include "math/Transform.h"

namespace BHive
{
	template <typename A>
	inline void CEREAL_SAVE_FUNCTION_NAME(A &ar, const FTransform &obj)
	{
		ar(cereal::make_nvp("Translation", obj.mTranslation), cereal::make_nvp("Rotation", obj.mRotation),
		   cereal::make_nvp("Scale", obj.mScale));
	}

	template <typename A>
	inline void CEREAL_LOAD_FUNCTION_NAME(A &ar, FTransform &obj)
	{
		ar(cereal::make_nvp("Translation", obj.mTranslation), cereal::make_nvp("Rotation", obj.mRotation),
		   cereal::make_nvp("Scale", obj.mScale));
		obj.calculate_model_matrix();
	}

} // namespace BHive