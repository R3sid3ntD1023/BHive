#include "VulkanImage.h"

namespace BHive
{
	VulkanImage::VulkanImage()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{

	}

	void VulkanImage::Create(
		uint32_t width, uint32_t height, uint32_t depth, vk::ImageType type, vk::ImageViewType viewType, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
		vk::SamplerCreateInfo samplerInfo)
	{
		mWidth = width;
		mHeight = height;
		mDepth = depth;
		mFormat = format;

		VulkanUtils::CreateImage(
			mWidth, mHeight, mDepth, type, format, vk::ImageTiling::eOptimal, usage,
			vk::MemoryPropertyFlagBits::eDeviceLocal,mTexture);

		VulkanUtils::CreateImageView(mTexture, viewType, format, aspect);

		VulkanUtils::CreateImageSampler(mTexture, samplerInfo);

		mDescriptor = vk::DescriptorImageInfo(mTexture.Sampler, mTexture.ImageView, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void VulkanImage::Upload(const void *data, size_t size)
	{
		Vulkan::AllocatedBuffer stagingBuffer;

		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer);

		void *map_memory = stagingBuffer.Memory.mapMemory(0, size);
		std::memcpy(map_memory, data, size);
		stagingBuffer.Memory.unmapMemory();

		Transition(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		VulkanUtils::CopyBufferToImage(stagingBuffer, mTexture, mWidth, mHeight);
		Transition(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void VulkanImage::Transition(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		VulkanUtils::TransitionImageLayout(mTexture.Image, oldLayout, newLayout);
	}
} // namespace BHive