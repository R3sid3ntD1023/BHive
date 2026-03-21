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
		mImage.Create(vk::ImageCreateFlagBits::eCubeCompatible ,size, size, 1, vk::ImageType::e2D, vk::ImageViewType::eCube, Convert(mCreateInfo));
	}

} // namespace BHive