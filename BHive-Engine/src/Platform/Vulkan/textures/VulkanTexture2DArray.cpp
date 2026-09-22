#include "VulkanTexture2DArray.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "Platform/Vulkan/VulkanConversions.h"

namespace BHive
{
	VulkanTexture2DArray::VulkanTexture2DArray(const glm::uvec2 &size, const FTextureCreateInfo &createInfo)
		: mSize(size),
		  mCreateInfo(createInfo)
	{
		auto format = ToVkFormat(mCreateInfo.Format);
		auto levels = mCreateInfo.MipLevels;
		auto layers = mCreateInfo.ArrayLayers;
		auto extent = vk::Extent3D(mSize.x, mSize.y, 1);
		auto usage = InferImageUsage(mCreateInfo.Roles);
		auto aspect = ToVkAspect(mCreateInfo.Aspect);
		auto magFilter = ToVkFilter(mCreateInfo.MagFilter);
		auto minFilter = ToVkFilter(mCreateInfo.MinFilter);
		auto addressMode = ToVkWrap(mCreateInfo.WrapMode);
		auto compare_enabled = mCreateInfo.CompareOp.has_value();
		auto compare_op = compare_enabled ? ToVkCompare(mCreateInfo.CompareOp.value()) : vk::CompareOp::eAlways;
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);

		vk::ImageCreateInfo imgInfo(
			{}, vk::ImageType::e2D, format, extent, levels, layers, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, 0
		);

		vk::ImageViewCreateInfo viewInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e2DArray, format, {}, range);

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

	void VulkanTexture2DArray::SetData(const FTextureUploadInfo &info)
	{
		size_t size = mSize.x * mSize.y * GetBytesPerPixel(mCreateInfo.Format);

		glm::uvec3 extents = glm::compMul(info.Extent) == 0 ? glm::uvec3{mSize, 1} : info.Extent;
		ImageCopyRegion region{.BaseArrayLayer = info.BaseArrayLayer, .LayerCount = info.Layers, .Offset = info.Offset, .Extents = extents};
		ImageSubresourceRange range{info.BaseMipLevel, info.Levels, info.BaseArrayLayer, info.Layers};
		mImage.Upload(info.Data, size, region, range);
	}

	VkImageView VulkanTexture2DArray::ResolveRenderView(uint32_t layer, uint32_t mip) const
	{
		return mImage.GetView(layer, mip);
	}

	void VulkanTexture2DArray::DebugPrintState()
	{
	}

} // namespace BHive