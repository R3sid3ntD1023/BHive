#include "VulkanTexture2DArray.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "Platform/Vulkan/VulkanBackend.h"

namespace BHive
{
	VulkanTexture2DArray::VulkanTexture2DArray(const glm::uvec2& size, const FTextureCreateInfo &createInfo)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size),
		  mCreateInfo(createInfo)
	{
		mImage.Create(size.x, size.y, 1, vk::ImageType::e2D, vk::ImageViewType::e2DArray, Convert(mCreateInfo));
	}

	void VulkanTexture2DArray::SetData(const FTextureUploadInfo &info)
	{
		size_t size = mSize.x * mSize.y * GetBytesPerPixel(mCreateInfo.Format);

		glm::uvec3 extents = glm::compMul(info.Extent) == 0 ? glm::uvec3{mSize, 1} : info.Extent;
		ImageCopyRegion region{.BaseArrayLayer = info.ArrayLayer, .LayerCount = info.LayerCount, .Offset = info.Offset, .Extents = extents};
		ImageSubresource sub{.MipLevel = info.MipLevel, .BaseArrayLayer = info.ArrayLayer, .LayerCount = info.LayerCount};
		mImage.Upload(info.Data, size, region, sub);
	}

} // namespace BHive