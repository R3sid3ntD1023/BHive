#pragma once

#include "Color.h"
#include "core/reflection/Reflection.h"
#include "gfx/Enumerations.h"

namespace BHive
{
	
	struct FTextureCreateInfo
	{
		uint32_t Levels = 1;
		uint32_t Channels = 0;
		bool GenerateMipMaps = false;

		EFormat Format = EFormat::None;
		EWrapMode WrapMode = EWrapMode::REPEAT;
		EMinFilter MinFilter = EMinFilter::LINEAR;
		EMagFilter MagFilter = EMagFilter::LINEAR;

		std::optional<ECompareMode> CompareMode;
		std::optional<ECompareOp> CompareOp;

		float BorderColor[4] = {0, 0, 0, 0};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(MAKE_NVP(Channels))
			ar(MAKE_NVP(Levels));
			ar(MAKE_NVP(GenerateMipMaps));
			ar(MAKE_NVP(Format));
			ar(MAKE_NVP(MinFilter));
			ar(MAKE_NVP(MagFilter));
			ar(MAKE_NVP(CompareMode));
			ar(MAKE_NVP(CompareOp));
			ar(MAKE_NVP(BorderColor));
		}

		REFLECTABLE()
	};

	REFLECT_EXTERN(FTextureCreateInfo)

} // namespace BHive
