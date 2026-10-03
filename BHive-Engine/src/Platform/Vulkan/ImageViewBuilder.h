#pragma once

#include "VulkanMemory.h"

namespace BHive
{
	class ImageViewBuilder
	{
	public:
		static vk::raii::ImageView &GetOrCreateFullView(ImageViews &views, const vk::ImageViewCreateInfo &base, uint32_t layers, uint32_t levels);

		static vk::raii::ImageView &GetOrCreateView(ImageViews &views, const vk::ImageViewCreateInfo &base, uint32_t layers, uint32_t levels, uint32_t layer, uint32_t mip);
	};
} // namespace BHive