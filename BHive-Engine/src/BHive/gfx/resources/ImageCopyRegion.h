#pragma once

namespace BHive
{
	struct ImageCopyRegion
	{
		uint32_t BaseArrayLayer = 0;
		uint32_t LayerCount = 1;
		glm::ivec3 Offset = {0, 0, 0};
		glm::ivec3 Extents = {0, 0, 1};
	};
}