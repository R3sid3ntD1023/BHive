#pragma once

#include "ImageInfo.h"
#include "RenderCommandList.h"


namespace BHive
{
	struct FPhase
	{
		std::string Name;

		FRenderCommandList CommandList;

		std::vector<FImageInfo> ImageUsages;
	};

}