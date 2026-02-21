#pragma once

#include "VulkanBackend.h"

namespace BHive
{
	class IVulkanTexture
	{
	public:

		virtual const Vulkan::AllocatedImage &GetImage() const = 0;

		virtual Vulkan::AllocatedImage &GetImage() = 0;
	};
}