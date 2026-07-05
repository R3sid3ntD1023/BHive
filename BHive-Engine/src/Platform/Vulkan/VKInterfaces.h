#pragma once

#include "VulkanCore.h"

namespace BHive
{
	class IVulkanTextureInterface
	{
	public:
		virtual ~IVulkanTextureInterface() = default;
		virtual VkImageView ResolveRenderView(uint32_t layer = 0, uint32_t mip = 0) const = 0;
	};
}