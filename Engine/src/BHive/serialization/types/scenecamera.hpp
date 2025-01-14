#pragma once

#include <cereal/cereal.hpp>
#include "scene/SceneCamera.h"

namespace BHive
{
	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, SceneCamera::FPerspectiveSettings &obj)
	{
		ar(cereal::make_nvp("Fov", obj.mFov), cereal::make_nvp("Near", obj.mNear), cereal::make_nvp("Far", obj.mFar));
	}

	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, SceneCamera::FOrthographicSettings &obj)
	{
		ar(cereal::make_nvp("Size", obj.mSize), cereal::make_nvp("Near", obj.mNear), cereal::make_nvp("Far", obj.mFar));
	}

	template <typename A>
	inline void CEREAL_SAVE_FUNCTION_NAME(A &ar, const SceneCamera &obj)
	{
		ar(cereal::make_nvp("ProjectionType", obj.mProjectionType),
		   cereal::make_nvp("PerspectiveSettings", obj.mPerspectiveSettings),
		   cereal::make_nvp("OrthographicSettings", obj.mOrthographicSettings));
	}

	template <typename A>
	inline void CEREAL_LOAD_FUNCTION_NAME(A &ar, SceneCamera &obj)
	{
		ar(cereal::make_nvp("ProjectionType", obj.mProjectionType),
		   cereal::make_nvp("PerspectiveSettings", obj.mPerspectiveSettings),
		   cereal::make_nvp("OrthographicSettings", obj.mOrthographicSettings));

		obj.RecalculateProjection();
	}
} // namespace BHive