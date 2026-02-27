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
		mImage.Create(size, size, size, vk::ImageType::e3D, vk::ImageViewType::eCubeArray, Convert(mCreateInfo));
	}

	void VulkanTextureCubeArray::SetData(const FTextureUploadInfo &info)
	{
		
	}

} // namespace BHive