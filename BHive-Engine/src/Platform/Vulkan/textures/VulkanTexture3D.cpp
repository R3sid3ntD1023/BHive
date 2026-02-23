#include "VulkanTexture3D.h"
#include "Platform/Vulkan/VulkanConverters.h"

namespace BHive
{

	VulkanTexture3D::VulkanTexture3D(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &create_info, const void *data)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mCreateInfo(create_info)
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
		mImage.Create(width, height, depth, vk::ImageType::e3D, vk::ImageViewType::e3D, api_info.Format, api_info.Usage, api_info.Aspect,
			sampler_info);
	}

} // namespace BHive