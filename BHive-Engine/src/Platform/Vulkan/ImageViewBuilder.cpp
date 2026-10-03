#include "ImageViewBuilder.h"
#include "VulkanBackend.h"

namespace BHive
{
	vk::raii::ImageView &ImageViewBuilder::GetOrCreateFullView(ImageViews &views, const vk::ImageViewCreateInfo &base, uint32_t layers, uint32_t levels)
	{
		if (views.FullView == VK_NULL_HANDLE)
		{
			auto viewInfo = base;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = levels;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = layers;
			views.FullView = VulkanBackend::GetLogicalDevice().createImageView(viewInfo);
		}

		return views.FullView;
	}

	vk::raii::ImageView &ImageViewBuilder::GetOrCreateView(ImageViews &views, const vk::ImageViewCreateInfo &base, uint32_t layers, uint32_t levels, uint32_t layer, uint32_t mip)
	{
		ASSERT(layer < layers, "Image view layer {} is out of range {}", layer, layers);
		ASSERT(mip < levels, "Image view mip {} is out of range {}", mip, levels);

		ViewKey key{layer, mip};
		if (auto found = views.Views.find(key); found != views.Views.end())
			return found->second;

		auto viewInfo = base;
		viewInfo.subresourceRange.baseMipLevel = mip;
		viewInfo.subresourceRange.levelCount = 1;

		switch (base.viewType)
		{
		case vk::ImageViewType::eCube:
			ASSERT(layer == 0, "Cube image views use layer 0");
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 6;
			break;
		case vk::ImageViewType::eCubeArray:
			viewInfo.viewType = vk::ImageViewType::e2D;
			viewInfo.subresourceRange.baseArrayLayer = layer;
			viewInfo.subresourceRange.layerCount = 1;
			break;
		case vk::ImageViewType::e1DArray:
			viewInfo.viewType = vk::ImageViewType::e1D;
			viewInfo.subresourceRange.baseArrayLayer = layer;
			viewInfo.subresourceRange.layerCount = 1;
			break;
		case vk::ImageViewType::e2DArray:
			viewInfo.viewType = vk::ImageViewType::e2D;
			viewInfo.subresourceRange.baseArrayLayer = layer;
			viewInfo.subresourceRange.layerCount = 1;
			break;
		default:
			ASSERT(layer == 0, "Non-array image views use layer 0");
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			break;
		}

		return views.Views.emplace(key, VulkanBackend::GetLogicalDevice().createImageView(viewInfo)).first->second;
	}
} // namespace BHive