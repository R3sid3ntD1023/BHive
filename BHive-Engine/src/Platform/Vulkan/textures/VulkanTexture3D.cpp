#include "VulkanTexture3D.h"

namespace BHive
{

	VulkanTexture3D::VulkanTexture3D(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &create_info, const void *data)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mInfo(create_info),
		  mCreateInfo(create_info)
	{
		auto channels = mCreateInfo.Channels;
		auto mag_filter = (vk::Filter)mInfo.FilterModes[0];
		auto min_filter = (vk::Filter)mInfo.FilterModes[1];
		auto wrap_mode = (vk::SamplerAddressMode)mInfo.WrapMode;
		auto compare_enabled = (vk::Bool32)mInfo.CompareMode;
		auto compare_operation = (vk::CompareOp)mInfo.CompareFunc;
		auto format = (vk::Format)mInfo.InternalFormat;


		vk::SamplerCreateInfo sampler_info({}, min_filter, mag_filter, vk::SamplerMipmapMode::eLinear, wrap_mode, wrap_mode, wrap_mode, 0, 0, 1, compare_enabled, compare_operation);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		mImage.Create(
			width, height, depth, vk::ImageType::e3D, vk::ImageViewType::e3D, format, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::ImageAspectFlagBits::eColor,
			sampler_info);
	}

	NativeHandle VulkanTexture3D::GetNativeHandle() const
	{
		return NativeHandle{.Ptr = &mImage.GetDescriptor()};
	}
} // namespace BHive