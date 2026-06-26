#include "VulkanTexture2DArray.h"
#include "Platform/Vulkan/VulkanConversions.h"
#include "Platform/Vulkan/VulkanBackend.h"

namespace BHive
{
	VulkanTexture2DArray::VulkanTexture2DArray(const glm::uvec2& size, const FTextureCreateInfo &createInfo)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size),
		  mCreateInfo(createInfo)
	{
		auto format = ToVkFormat(mCreateInfo.Format);
		auto levels = mCreateInfo.MipLevels;
		auto layers = mCreateInfo.ArrayLayers;
		auto extent = vk::Extent3D(mSize.x, mSize.y, 1);
		auto usage = InferImageUsage(mCreateInfo.Roles);

		ImageCreateInfo create_info{};
		create_info.ImageCI = vk::ImageCreateInfo({}, vk::ImageType::e2D, format, extent, levels, layers,
			vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, 0);

		//create default view info image is set in VulkanImage
		auto aspect = ToVkAspect(mCreateInfo.Aspect);
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);
		create_info.ViewCI = vk::ImageViewCreateInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e2DArray, format, {}, range);
		
		//create sampler info
		auto magFilter = ToVkFilter(mCreateInfo.MagFilter);
		auto minFilter = ToVkFilter(mCreateInfo.MinFilter);
		auto addressMode = ToVkWrap(mCreateInfo.WrapMode);
		auto compare_enabled = mCreateInfo.CompareOp.has_value();
		auto compare_op =  compare_enabled ? ToVkCompare(mCreateInfo.CompareOp.value()) : vk::CompareOp::eAlways;

		create_info.SamplerCI =
			vk::SamplerCreateInfo({}, magFilter, minFilter, vk::SamplerMipmapMode::eLinear, addressMode, addressMode, addressMode,  0.0f, 0u, 1.0f, compare_enabled, compare_op, 0.0f, 0.0f, vk::BorderColor::eIntOpaqueBlack, VK_FALSE);
		create_info.DebugName = mCreateInfo.DebugName;
		create_info.BytesPerPixel = GetBytesPerPixel(mCreateInfo.Format);
		create_info.ViewTopology = EViewTopology::Mips2DArray;

		mImage.Initialize(create_info);
	}

	void VulkanTexture2DArray::GenerateMips()
	{
		mImage.GenerateMipMaps(mSize.x, mSize.y, mCreateInfo.ArrayLayers, mCreateInfo.MipLevels);
	}

	void VulkanTexture2DArray::SetData(const FTextureUploadInfo &info)
	{
		size_t size = mSize.x * mSize.y * GetBytesPerPixel(mCreateInfo.Format);

		glm::uvec3 extents = glm::compMul(info.Extent) == 0 ? glm::uvec3{mSize, 1} : info.Extent;
		ImageCopyRegion region{.BaseArrayLayer = info.BaseArrayLayer, .LayerCount = info.Layers, .Offset = info.Offset, .Extents = extents};
		ImageSubresourceRange range{info.BaseMipLevel, info.Levels, info.BaseArrayLayer, info.Layers};
		mImage.Upload(info.Data, size, region, range);
	}

	NativeHandle VulkanTexture2DArray::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		VkImageView view = mImage.Native().GetLayerMipView(layer, mip);
		return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(view));
	}

	void VulkanTexture2DArray::DebugPrintState()
	{
		mImage.DebugPrintState();
	}

} // namespace BHive