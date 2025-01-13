#pragma once

#include <cereal/cereal.hpp>
#include "gfx/TextureSpecification.h"

namespace BHive
{
	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, FTextureSpecification &spec)
	{
		ar(cereal::make_nvp("Format", spec.mFormat), cereal::make_nvp("Channels", spec.mChannels),
		   cereal::make_nvp("WrapMode", spec.mWrapMode), cereal::make_nvp("MinFilter", spec.mMinFilter),
		   cereal::make_nvp("MagFilter", spec.mMagFilter), cereal::make_nvp("BorderColor", spec.mBorderColor),
		   cereal::make_nvp("Mips", spec.mMips), cereal::make_nvp("Type", spec.mType),
		   cereal::make_nvp("Levels", spec.mLevels), cereal::make_nvp("CompareMode", spec.mCompareMode),
		   cereal::make_nvp("CompareFunc", spec.mCompareFunc));
	}
} // namespace BHive
