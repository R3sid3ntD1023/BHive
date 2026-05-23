#include "VulkanTexture3D.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "Platform/Vulkan/VulkanBackend.h"


namespace BHive
{

	VulkanTexture3D::VulkanTexture3D(const glm::uvec3 &size, const FTextureCreateInfo &createInfo, const Buffer &data)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size),
		  mCreateInfo(createInfo)
	{

		auto format = ToVkFormat(mCreateInfo.Format);
		auto levels = mCreateInfo.MipLevels;
		auto layers = mCreateInfo.ArrayLayers;
		auto extent = vk::Extent3D(mSize.x, mSize.y, mSize.z);
		auto usage = InferImageUsage(mCreateInfo.Roles);

		ImageCreateInfo create_info{};
		create_info.ImageCI =
			vk::ImageCreateInfo({}, vk::ImageType::e3D, format, extent, levels, layers, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, 0);

		// create default view info image is set in VulkanImage
		auto aspect = ToVkAspect(mCreateInfo.Aspect);
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);
		create_info.ViewCI = vk::ImageViewCreateInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e3D, format, {}, range);

		// create sampler info
		auto magFilter = ToVkFilter(mCreateInfo.MagFilter);
		auto minFilter = ToVkFilter(mCreateInfo.MinFilter);
		auto addressMode = ToVkWrap(mCreateInfo.WrapMode);
		auto compare_enabled = mCreateInfo.CompareOp.has_value();
		auto compare_op = compare_enabled ? ToVkCompare(mCreateInfo.CompareOp.value()) : vk::CompareOp::eAlways;

		create_info.SamplerCI = vk::SamplerCreateInfo(
			{}, magFilter, minFilter, vk::SamplerMipmapMode::eLinear, addressMode, addressMode, addressMode, 0.0f, 0u, 1.0f, compare_enabled, compare_op, 0.0f, 0.0f, vk::BorderColor::eIntOpaqueBlack,
			VK_FALSE);
		create_info.DebugName = mCreateInfo.DebugName;
		create_info.BytesPerPixel = GetBytesPerPixel(mCreateInfo.Format);
		create_info.ViewTopology = EViewTopology::Mips3D;
		mImage.Initialize(create_info);
	}

	NativeHandle VulkanTexture3D::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		VkImageView view = mImage.Native().GetMipView(mip);
		return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(view));
	}

} // namespace BHive