#pragma once

#include <cereal/cereal.hpp>
#include "mesh/AnimationFrames.h"

namespace BHive
{

	template <typename A, typename T>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, TKeyFrame<T> &obj)
	{
		ar(cereal::make_nvp("Value", obj.mValue), cereal::make_nvp("TimeStamp", obj.mTimeStamp));
	}

	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, FrameData &obj)
	{
		ar(cereal::make_nvp("Positions", obj.mPositions), cereal::make_nvp("Rotations", obj.mRotations),
		   cereal::make_nvp("Scales", obj.mScales));
	}

} // namespace BHive