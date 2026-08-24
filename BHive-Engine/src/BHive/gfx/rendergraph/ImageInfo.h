#pragma once

#include "core/Core.h"
#include "gfx/resources/ImageSubResourceRange.h"
#include "gfx/Enumerations.h";

namespace BHive
{
	class Texture;

	// Tex, Mip, Levels, Layer, Layers, Access
	struct FImageInfo
	{
		Ref<Texture> Texture;
		EImageUsage Access = EImageUsage::None;
		ImageSubresourceRange Range{};
	};
}