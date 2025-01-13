#pragma once

#include <cereal/cereal.hpp>
#include "audio/AudioSpecification.h"

namespace BHive
{
	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, FAudioSpecification &obj)
	{
		ar(cereal::make_nvp("Format", obj.mFormat), cereal::make_nvp("NumSamples", obj.mNumSamples),
		   cereal::make_nvp("SampleRate", obj.mSampleRate), cereal::make_nvp("StartLoop", obj.mStartLoop),
		   cereal::make_nvp("EndLoop", obj.mEndLoop));
	}

} // namespace BHive