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

		void Create(uint32_t width, uint32_t height, uint32_t depth, vk::ImageType type, vk::ImageViewType viewType, const FVulkanTextureCreateInfo &createInfo);

		void Upload(const void *data, size_t size, const ImageCopyRegion &region = {}, const ImageSubresource &sub = {});

		const AllocatedImage &GetImage() const { return mImage; };

		AllocatedImage &GetImage() { return mImage; };

		const vk::DescriptorImageInfo GetDescriptor() const;

		NativeHandle GetNativeHandle() const { return NativeHandle::FromPtr(&mImage); }

	private:
		AllocatedImage mImage{};
		vk::Format mFormat = vk::Format::eUndefined;
	};
}