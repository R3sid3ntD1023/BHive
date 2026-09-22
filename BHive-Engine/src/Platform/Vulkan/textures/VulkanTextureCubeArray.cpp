#include "VulkanTextureCubeArray.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "Platform/Vulkan/VulkanConversions.h"

namespace BHive
{
	VulkanTextureCubeArray::VulkanTextureCubeArray(uint32_t size, const FTextureCreateInfo &createInfo)
		: mSize(size),
		  mCreateInfo(createInfo)
	{
		auto layers = mCreateInfo.ArrayLayers * 6;
		auto format = ToVkFormat(mCreateInfo.Format);
		auto levels = mCreateInfo.MipLevels;
		auto extent = vk::Extent3D(mSize, mSize, 1);
		auto usage = InferImageUsage(mCreateInfo.Roles);
		auto aspect = ToVkAspect(mCreateInfo.Aspect);
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);
		auto magFilter = ToVkFilter(mCreateInfo.MagFilter);
		auto minFilter = ToVkFilter(mCreateInfo.MinFilter);
		auto addressMode = ToVkWrap(mCreateInfo.WrapMode);
		auto compare_enabled = mCreateInfo.CompareOp.has_value();
		auto compare_op = compare_enabled ? ToVkCompare(mCreateInfo.CompareOp.value()) : vk::CompareOp::eAlways;

		vk::ImageCreateInfo imgInfo(
			vk::ImageCreateFlagBits::eCubeCompatible,
			vk::ImageType::e2D,
			format,
			extent,
			levels,
			layers,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			usage,
			vk::SharingMode::eExclusive,
			0
		);

		vk::ImageViewCreateInfo viewInfo({}, VK_NULL_HANDLE, vk::ImageViewType::eCubeArray, format, {}, range);

		vk::SamplerCreateInfo smpInfo(
			{},
			magFilter,
			minFilter,
			vk::SamplerMipmapMode::eLinear,
			addressMode,
			addressMode,
			addressMode,
			0.0f,
			0u,
			1.0f,
			compare_enabled,
			compare_op,
			0.0f,
			float(levels - 1),
			ToVkBorderColor(mCreateInfo.BorderColor),
			VK_FALSE
		);

		mImage.Initialize(imgInfo, viewInfo, smpInfo);
		mImage.SetDebugName(mCreateInfo.DebugName);
	}

	void VulkanTextureCubeArray::SetData(const FTextureUploadInfo &info)
	{
	}

	VkImageView VulkanTextureCubeArray::ResolveRenderView(uint32_t layer, uint32_t mip) const
	{
		return mImage.GetView(layer, mip);
	}

	void VulkanTextureCubeArray::DebugPrintState()
	{
	}

} // namespace BHive