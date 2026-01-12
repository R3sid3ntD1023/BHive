#include "gfx/utils/texture/TextureUtils.h"
#include "gfx/VulkanUtils.h"
#include "TextureCubeArray.h"

namespace BHive
{
	TextureCubeArray::TextureCubeArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &spec)
		: mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mCreateInfo(spec),
		  mInfo(spec)
	{
		auto channels = mCreateInfo.Channels;
		auto mag_filter = (vk::Filter)mInfo.FilterModes[0];
		auto min_filter = (vk::Filter)mInfo.FilterModes[1];
		auto wrap_mode = (vk::SamplerAddressMode)mInfo.WrapMode;
		auto compare_enabled = (vk::Bool32)mInfo.CompareMode;
		auto compare_operation = (vk::CompareOp)mInfo.CompareFunc;
		auto format = (vk::Format)mInfo.InternalFormat;

		VulkanUtils::CreateImage(
			width, height, 1, vk::ImageType::e3D, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal, mTextureHandle);

		VulkanUtils::CreateImageView(mTextureHandle, vk::ImageViewType::eCubeArray, format);

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

	TextureCubeArray::~TextureCubeArray()
	{
	}

	void TextureCubeArray::Bind(uint32_t slot) const
	{
	}

	void TextureCubeArray::UnBind(uint32_t slot) const
	{
	}

	void TextureCubeArray::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
	}
} // namespace BHive