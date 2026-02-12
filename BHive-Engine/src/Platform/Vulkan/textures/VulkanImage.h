#pragma once

#include "Platform/Vulkan/VulkanBackend.h"

namespace BHive
{
	class VulkanImage
	{
	public:
		VulkanImage();

		void
		Create(uint32_t width, uint32_t height, uint32_t depth, vk::ImageType type, vk::ImageViewType viewType, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect, vk::SamplerCreateInfo samplerInfo = {});

		void Upload(const void *data, size_t size);

		void Transition(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

		const Vulkan::AllocatedTexture &GetAllocatedTexture() { return mTexture; };

		const vk::DescriptorImageInfo &GetDescriptor() const { return mDescriptor; }

		uint32_t GetWidth() const { return mWidth; }

		uint32_t GetHeight() const { return mHeight; }

		uint32_t GetDepth() const { return mDepth; }

	private:
		vk::raii::Device &mDevice;
		Vulkan::AllocatedTexture mTexture{};
		vk::DescriptorImageInfo mDescriptor{};
		uint32_t mWidth = 0, mHeight = 0, mDepth = 0;
		vk::Format mFormat = vk::Format::eUndefined;
	};
}