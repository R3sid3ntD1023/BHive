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
		mCreateInfo.ArrayLayers *= 6;
		mImage.Create(vk::ImageCreateFlagBits::eCubeCompatible , size, size, size, vk::ImageType::e3D, vk::ImageViewType::eCubeArray, Convert(mCreateInfo));
	}

	void VulkanTextureCubeArray::SetData(const FTextureUploadInfo &info)
	{
		
	}

	NativeHandle VulkanTextureCubeArray::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		return NativeHandle::FromPtr(&mImage.GetNativeHandle().As<AllocatedImage>()->GetView());
	}

} // namespace BHive