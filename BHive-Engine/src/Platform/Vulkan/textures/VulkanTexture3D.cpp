#include "VulkanTexture3D.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "Platform/Vulkan/VulkanConversions.h"

namespace BHive
{

	VulkanTexture3D::VulkanTexture3D(const glm::uvec3 &size, const FTextureCreateInfo &createInfo, const ByteBuffer &data)
		: mSize(size),
		  mCreateInfo(createInfo)
	{

		auto format = ToVkFormat(mCreateInfo.Format);
		auto levels = mCreateInfo.MipLevels;
		auto layers = mCreateInfo.ArrayLayers;
		auto extent = vk::Extent3D(mSize.x, mSize.y, mSize.z);
		auto usage = InferImageUsage(mCreateInfo.Roles);
		auto aspect = ToVkAspect(mCreateInfo.Aspect);
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);
		auto magFilter = ToVkFilter(mCreateInfo.MagFilter);
		auto minFilter = ToVkFilter(mCreateInfo.MinFilter);
		auto addressMode = ToVkWrap(mCreateInfo.WrapMode);
		auto compare_enabled = mCreateInfo.CompareOp.has_value();
		auto compare_op = compare_enabled ? ToVkCompare(mCreateInfo.CompareOp.value()) : vk::CompareOp::eAlways;

		vk::ImageCreateInfo imgInfo(
			{}, vk::ImageType::e3D, format, extent, levels, layers, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, 0
		);

		vk::ImageViewCreateInfo viewInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e3D, format, {}, range);

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

	VkImageView VulkanTexture3D::ResolveRenderView(uint32_t layer, uint32_t mip) const
	{
		return mImage.GetView(layer, mip);
	}

	void VulkanTexture3D::DebugPrintState()
	{
	}

} // namespace BHive