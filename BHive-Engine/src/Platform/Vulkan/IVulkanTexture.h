#pragma once

#include "VulkanMemory.h"

namespace BHive
{
	class IVulkanTexture
	{
	public:

		virtual const AllocatedImage &GetImage() const = 0;

		virtual AllocatedImage &GetImage() = 0;

		virtual const vk::DescriptorImageInfo GetDescriptor() const = 0;
	};
}