#pragma once

#include "Platform/Vulkan/VulkanMemory.h"
#include "Platform/Vulkan/VulkanImageRegions.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "Platform/Vulkan/ImageViewBuilder.h"

namespace BHive
{
	struct ImageCreateInfo
	{
		vk::ImageCreateFlags CreateFlags{};
		uint32_t Width = 0, Height = 0, Depth = 1;
		vk::ImageType Type{};
		vk::ImageViewType ViewType{};
		FVulkanTextureCreateInfo CreateInfo{};
		EViewTopology ViewTopology;
	};

	class VulkanImage
	{
	public:
		virtual ~VulkanImage();

		void Initialize(const ImageCreateInfo &createInfo);

		void Upload(const void *data, size_t size, const ImageCopyRegion &region = {}, const ImageSubresource &sub = {});

		NativeHandle GetNativeHandle() const { return Handle::Image(&mImage); }

		NativeHandle GetNativeHandle() { return Handle::Image(&mImage); }

	private:
		GPUImage mImage{};
	};

}