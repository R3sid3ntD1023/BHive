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

		mImage.Create(size, size, size, vk::ImageType::e2D, vk::ImageViewType::eCube, Convert(mCreateInfo));
	}

} // namespace BHive