#pragma once

#include "Platform/Vulkan/VulkanMemory.h"
#include "Platform/Vulkan/VulkanImageRegions.h"
#include "Platform/Vulkan/VulkanConverters.h"

namespace BHive
{
	class VulkanImage
	{
	public:
		VulkanImage() = default;

		~VulkanImage();

		void Create(vk::ImageCreateFlags createFlags, uint32_t width, uint32_t height, uint32_t depth, vk::ImageType type, vk::ImageViewType viewType, const FVulkanTextureCreateInfo &createInfo);

		void Upload(const void *data, size_t size, const ImageCopyRegion &region = {}, const ImageSubresource &sub = {});

		NativeHandle GetNativeHandle() const { return Handle::Image(&mImage); }

		NativeHandle GetNativeHandle() { return Handle::Image(&mImage); }

	private:
		AllocatedImage mImage{};
	};
}