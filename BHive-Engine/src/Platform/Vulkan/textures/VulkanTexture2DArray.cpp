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
		ImageCreateInfo create_info{};
		create_info.Width = mSize.x;
		create_info.Height = mSize.y;
		create_info.Depth = 1;
		create_info.Type = vk::ImageType::e2D;
		create_info.ViewType = vk::ImageViewType::e2DArray;
		create_info.CreateInfo = Convert(mCreateInfo);
		create_info.ViewTopology = EViewTopology::Mips2DArray;
		create_info.CreateInfo.ArrayLayers = mCreateInfo.ArrayLayers;
		create_info.CreateInfo.MipLevels = mCreateInfo.MipLevels;

		mImage.Initialize(create_info);
	}

	void VulkanTexture2DArray::SetData(const FTextureUploadInfo &info)
	{
		size_t size = mSize.x * mSize.y * GetBytesPerPixel(mCreateInfo.Format);

		glm::uvec3 extents = glm::compMul(info.Extent) == 0 ? glm::uvec3{mSize, 1} : info.Extent;
		ImageCopyRegion region{.BaseArrayLayer = info.ArrayLayer, .LayerCount = info.LayerCount, .Offset = info.Offset, .Extents = extents};
		ImageSubresource sub{.MipLevel = info.MipLevel, .BaseArrayLayer = info.ArrayLayer, .LayerCount = info.LayerCount};
		mImage.Upload(info.Data, size, region, sub);
	}

	NativeHandle VulkanTexture2DArray::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		auto image = mImage.GetNativeHandle().As<GPUImage>();
		VkImageView view = image->GetLayerMipView(layer,  mip);
		return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(view));
	}

} // namespace BHive