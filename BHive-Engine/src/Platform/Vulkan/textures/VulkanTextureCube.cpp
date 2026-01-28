#include "gfx/utils/texture/TextureUtils.h"
#include "VulkanTextureCube.h"

namespace BHive
{
	VulkanTextureCube::VulkanTextureCube(uint32_t size, const FTextureCreateInfo &create_info)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mSize(size),
		  mCreateInfo(create_info),
		  mInfo(create_info)
	{
		auto channels = mCreateInfo.Channels;
		auto mag_filter = (vk::Filter)mInfo.FilterModes[0];
		auto min_filter = (vk::Filter)mInfo.FilterModes[1];
		auto wrap_mode = (vk::SamplerAddressMode)mInfo.WrapMode;
		auto compare_enabled = (vk::Bool32)mInfo.CompareMode;
		auto compare_operation = (vk::CompareOp)mInfo.CompareFunc;
		auto format = (vk::Format)mInfo.InternalFormat;

		VulkanUtils::CreateImage(
			size, size, 1, vk::ImageType::e2D, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal, mTextureHandle);

		VulkanUtils::CreateImageView(mTextureHandle, vk::ImageViewType::eCube, format);

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

} // namespace BHive