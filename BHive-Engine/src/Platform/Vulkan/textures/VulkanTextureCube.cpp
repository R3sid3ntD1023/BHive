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
		Initilaize();
	}

	void VulkanTextureCube::Initilaize()
	{
		ImageCreateInfo create_info{};
		create_info.CreateFlags = vk::ImageCreateFlagBits::eCubeCompatible;
		create_info.Width = mSize;
		create_info.Height = mSize;
		create_info.Depth = 1;
		create_info.Type = vk::ImageType::e2D;
		create_info.ViewType = vk::ImageViewType::eCube;
		create_info.CreateInfo = Convert(mCreateInfo);
		create_info.CreateInfo.ArrayLayers = 6;
		create_info.CreateInfo.MipLevels = mCreateInfo.MipLevels;
		create_info.ViewTopology = EViewTopology::Cube;
		mImage.Initialize(create_info);
	}

	NativeHandle VulkanTextureCube::GetRenderView(uint32_t face, uint32_t mip) const
	{
		auto image = mImage.GetNativeHandle().As<GPUImage>();
		VkImageView view = image->GetCubeFaceView(face, mip);
		return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(view));
	}

} // namespace BHive