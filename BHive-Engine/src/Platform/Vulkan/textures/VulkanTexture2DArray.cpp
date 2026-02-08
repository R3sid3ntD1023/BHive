#include "../VulkanCore.h"
#include "gfx/utils/texture/TextureUtils.h"
#include "VulkanTexture2DArray.h"

namespace BHive
{
	VulkanTexture2DArray::VulkanTexture2DArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mCreateInfo(specification),
		  mInfo(specification)
	{
		auto channels = mCreateInfo.Channels;
		auto mag_filter = (vk::Filter)mInfo.FilterModes[0];
		auto min_filter = (vk::Filter)mInfo.FilterModes[1];
		auto wrap_mode = (vk::SamplerAddressMode)mInfo.WrapMode;
		auto compare_enabled = (vk::Bool32)mInfo.CompareMode;
		auto compare_operation = (vk::CompareOp)mInfo.CompareFunc;
		auto format = (vk::Format)mInfo.InternalFormat;

		VulkanUtils::CreateImage(
			mWidth, mHeight, 1, vk::ImageType::e2D, format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal, mTextureHandle);

		VulkanUtils::CreateImageView(mTextureHandle, vk::ImageViewType::e2DArray, format);

		vk::SamplerCreateInfo sampler_info({}, min_filter, mag_filter, vk::SamplerMipmapMode::eLinear, wrap_mode, wrap_mode, wrap_mode, 0, 0, 1, compare_enabled, compare_operation);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		VulkanUtils::CreateImageSampler(mTextureHandle, sampler_info);

		mDescriptorInfo = VulkanUtils::CreateDescriptorImageInfo(mTextureHandle, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void VulkanTexture2DArray::Bind(uint32_t slot) const
	{
	}

	void VulkanTexture2DArray::UnBind(uint32_t slot) const
	{
	}

	void VulkanTexture2DArray::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{

		vk::DeviceSize size = mWidth * mHeight * mDepth * mCreateInfo.Channels;

		AllocatedVulkanBuffer stagingBuffer;
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer);
		//stagingBuffer.SetData(data, size, offsetX);

		auto &image = mTextureHandle.Image;
		VulkanUtils::TransitionImageLayout(image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		VulkanUtils::CopyBufferToImage(stagingBuffer, mTextureHandle, mWidth, mHeight);
		VulkanUtils::TransitionImageLayout(image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}
} // namespace BHive