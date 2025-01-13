#pragma once

#include <cereal/cereal.hpp>
#include "renderers/Lights.h"


namespace BHive
{
	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, Light &obj)
	{
		ar(cereal::make_nvp("Color", obj.mColor), cereal::make_nvp("Brightness", obj.mBrightness));
	}

	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, PointLight &obj)
	{
		ar(cereal::base_class<Light>(&obj));
		ar(cereal::make_nvp("Radius", obj.mRadius));
	}

	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, SpotLight &obj)
	{
		ar(cereal::base_class<PointLight>(&obj));
		ar(cereal::make_nvp("InnerCutoff", obj.mInnerCutOff), cereal::make_nvp("OuterCutoff", obj.mOuterCutOff));
	}

} // namespace BHive