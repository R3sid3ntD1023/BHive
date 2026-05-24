#pragma once

#include "Color.h"
#include "core/reflection/Reflection.h"
#include "gfx/Enumerations.h"

namespace BHive
{;

	struct FTextureCreateInfo
	{
		ETextureRole Roles = ETextureRole::Sampled | ETextureRole::TransferDst | ETextureRole::TransferSrc;

		ETextureAspect Aspect = ETextureAspect::Color;

		EFormat Format = EFormat::None;

		EWrapMode WrapMode = EWrapMode::REPEAT;

		EMinFilter MinFilter = EMinFilter::LINEAR;

		EMagFilter MagFilter = EMagFilter::LINEAR;

		std::optional<ECompareMode> CompareMode;

		std::optional<ECompareOp> CompareOp;

		uint32_t MipLevels = 1;

		uint32_t ArrayLayers = 1;

		std::string DebugName = "*";

		template <typename A>
		void Serialize(A &ar)
		{		
			ar(MAKE_NVP(Roles));
			ar(MAKE_NVP(Aspect));
			ar(MAKE_NVP(Format));
			ar(MAKE_NVP(WrapMode));
			ar(MAKE_NVP(MinFilter));
			ar(MAKE_NVP(MagFilter));
			ar(MAKE_NVP(CompareMode));
			ar(MAKE_NVP(CompareOp));
			ar(MAKE_NVP(MipLevels));
			ar(MAKE_NVP(ArrayLayers));
		}

		REFLECTABLE()
	};

	REFLECT_EXTERN(FTextureCreateInfo)

} // namespace BHive
