#include "VulkanTexture2DArray.h"
#include "Platform/Vulkan/VulkanConverters.h"

namespace BHive
{
	VulkanTexture2DArray::VulkanTexture2DArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mCreateInfo(specification)
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

		mImage.Create(width, height, depth, vk::ImageType::e2D, vk::ImageViewType::e2DArray, api_info.Format, api_info.Usage, api_info.Aspect,
			sampler_info);
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

		mImage.Upload(data, size);
	}
	NativeHandle VulkanTexture2DArray::GetNativeHandle() const
	{
		return mImage.GetNativeHandle();
	}
} // namespace BHive