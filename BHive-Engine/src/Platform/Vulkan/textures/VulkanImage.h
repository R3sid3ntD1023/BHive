#pragma once

#include "Platform/Vulkan/VulkanBackend.h"
#include "Platform/Vulkan/IVulkanTexture.h"

namespace BHive
{
	class VulkanImage
	{
	public:
		VulkanImage();

		~VulkanImage();

		void Create(
			uint32_t width, uint32_t height, uint32_t depth, uint32_t layers, vk::ImageType type, vk::ImageViewType viewType, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
			vk::SamplerCreateInfo samplerInfo = {});

		void Upload(const void *data, size_t size, const ImageCopyRegion &region = {}, const ImageSubresource &sub = {});

		const Vulkan::AllocatedImage &GetImage() const { return mImage; };

		Vulkan::AllocatedImage &GetImage() { return mImage; };

		const vk::DescriptorImageInfo GetDescriptor() const;

		NativeHandle GetNativeHandle() const { return NativeHandle::FromPtr(&mImage); }

		uint32_t GetWidth() const { return mWidth; }

		uint32_t GetHeight() const { return mHeight; }

		uint32_t GetDepth() const { return mDepth; }

	private:
		vk::raii::Device &mDevice;
		Vulkan::AllocatedImage mImage{};
		uint32_t mWidth = 0, mHeight = 0, mDepth = 0;
		vk::Format mFormat = vk::Format::eUndefined;
	};
}