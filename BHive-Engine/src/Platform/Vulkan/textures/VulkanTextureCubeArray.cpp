#include "VulkanTextureCubeArray.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "Platform/Vulkan/VulkanBackend.h"

namespace BHive
{
	VulkanTextureCubeArray::VulkanTextureCubeArray(uint32_t size, const FTextureCreateInfo &createInfo)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size),
		  mCreateInfo(createInfo)
	{
		uint32_t cubeCount = mCreateInfo.ArrayLayers;

		ImageCreateInfo create_info{};
		create_info.CreateFlags = vk::ImageCreateFlagBits::eCubeCompatible;
		create_info.Width = mSize;
		create_info.Height = mSize;
		create_info.Depth = 1;
		create_info.Type = vk::ImageType::e3D;
		create_info.ViewType = vk::ImageViewType::eCubeArray;
		create_info.CreateInfo = Convert(mCreateInfo);
		create_info.CreateInfo.ArrayLayers = cubeCount * 6;
		create_info.CreateInfo.MipLevels = mCreateInfo.MipLevels;
		create_info.ViewTopology = EViewTopology::CubeArray;
		
		mImage.Initialize(create_info);
	}

	void VulkanTextureCubeArray::SetData(const FTextureUploadInfo &info)
	{
		
	}

	NativeHandle VulkanTextureCubeArray::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		auto image = mImage.GetNativeHandle().As<GPUImage>();
		VkImageView view = image->GetLayerMipView(layer, mip);
		return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(view));
	}

} // namespace BHive