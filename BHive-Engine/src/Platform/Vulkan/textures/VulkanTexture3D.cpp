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
		ImageCreateInfo create_info{};
		create_info.Width = mSize.x;
		create_info.Height = mSize.y;
		create_info.Depth = mSize.z;
		create_info.Type = vk::ImageType::e3D;
		create_info.ViewType = vk::ImageViewType::e3D;
		create_info.CreateInfo = Convert(mCreateInfo);
		create_info.ViewTopology = EViewTopology::Mips3D;
		create_info.CreateInfo.ArrayLayers = mCreateInfo.ArrayLayers;
		create_info.CreateInfo.MipLevels = mCreateInfo.MipLevels;
		mImage.Initialize(create_info);
	}

	NativeHandle VulkanTexture3D::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		auto image = mImage.GetNativeHandle().As<GPUImage>();
		VkImageView view = image->GetMipView(mip);
		return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(view));
	}

} // namespace BHive