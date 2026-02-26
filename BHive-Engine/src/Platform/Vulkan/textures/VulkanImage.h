#pragma once

#include "Platform/Vulkan/VulkanMemory.h"
#include "Platform/Vulkan/VulkanImageRegions.h"

namespace BHive
{
	class VulkanImage
	{
	public:
		VulkanImage() = default;

		~VulkanImage();

		void Create(
			uint32_t width, uint32_t height, uint32_t depth, uint32_t layers, vk::ImageType type, vk::ImageViewType viewType, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
			vk::SamplerCreateInfo samplerInfo = {});

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