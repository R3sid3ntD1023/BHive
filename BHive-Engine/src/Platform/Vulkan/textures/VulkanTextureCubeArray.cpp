#include "VulkanTextureCubeArray.h"
#include "Platform/Vulkan/VulkanConverters.h"

namespace BHive
{
	VulkanTextureCubeArray::VulkanTextureCubeArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &spec)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mCreateInfo(spec)
	{
		auto api_info = Vulkan::Convert(mCreateInfo);

		vk::SamplerCreateInfo sampler_info(
			{}, api_info.MinFilter, api_info.MagFilter, vk::SamplerMipmapMode::eLinear, api_info.WrapMode, api_info.WrapMode, api_info.WrapMode, 0, 0, 1, api_info.CompareEnabled, api_info.CompareOp);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		mImage.Create(mWidth, mHeight, mDepth, vk::ImageType::e3D, vk::ImageViewType::eCubeArray, api_info.Format, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::ImageAspectFlagBits::eColor, sampler_info);
	}

	void VulkanTextureCubeArray::Bind(uint32_t slot) const
	{
	}

	void VulkanTextureCubeArray::UnBind(uint32_t slot) const
	{
	}

	void VulkanTextureCubeArray::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
		//mImage.Upload(data, size);
	}

	NativeHandle VulkanTextureCubeArray::GetNativeHandle() const
	{
		return mImage.GetNativeHandle();
	}
} // namespace BHive