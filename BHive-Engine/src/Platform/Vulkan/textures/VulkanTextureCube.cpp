#include "VulkanTextureCube.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "Platform/Vulkan/VulkanBackend.h"

namespace BHive
{
	VulkanTextureCube::VulkanTextureCube(uint32_t size, const FTextureCreateInfo &create_info)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size),
		  mCreateInfo(create_info)
	{
		mCreateInfo.ArrayLayers = 6;
		Initilaize();
	}

	void VulkanTextureCube::Initilaize()
	{
		LOG_ERROR("Cube CreateInfo.Usage = {}", (uint32_t)mCreateInfo.Usage);
		mImage.Create(vk::ImageCreateFlagBits::eCubeCompatible, mSize, mSize, 1, vk::ImageType::e2D, vk::ImageViewType::eCube, Convert(mCreateInfo));

		auto allocated_image = mImage.GetNativeHandle().As<AllocatedImage>();
		auto image = allocated_image->CreateImage();

		for (uint32_t face = 0; face < 6; face++)
		{
			ImageViewDesc desc{};
			desc.Type = vk::ImageViewType::e2D;
			desc.Format = ToVkFormat(mCreateInfo.Format);
			desc.BaseArrayLayer = face;
			desc.LayerCount = 1;
			desc.BaseMipLevel = 0;
			desc.LayerCount = 1;
			desc.Aspect = ToVkAspect(mCreateInfo.Aspect);

			mFaceViews[face] = VulkanBackend::GetGPUResourceManager().CreateImageView(allocated_image->GetImage(), desc);
		}
	}

	NativeHandle VulkanTextureCube::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		VkImageView view = VulkanBackend::GetGPUResourceManager().GetImageView(mFaceViews[layer]);
		return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(view));
	}

} // namespace BHive