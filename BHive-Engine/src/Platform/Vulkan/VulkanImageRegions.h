#pragma once

#include "core/Core.h"

namespace BHive
{
	struct ImageCopyRegion
	{
		uint32_t BaseArrayLayer = 0;
		uint32_t LayerCount = 1;
		glm::ivec3 Offset = {0, 0, 0};
		glm::ivec3 Extents = {0, 0, 1};
	};

	/// <summary>
	/// BaseMipLevel, Levels, BaseArrayLayer, Layers
	/// </summary>
	struct ImageSubresource
	{
		uint32_t BaseMipLevel = 0;
		uint32_t LevelCount = 1;
		uint32_t BaseArrayLayer = 0;
		uint32_t LayerCount = 1;
	};
} // namespace BHive