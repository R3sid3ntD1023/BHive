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
		mImage.Create({} , size.x, size.y, size.z, vk::ImageType::e3D, vk::ImageViewType::e3D, Convert(mCreateInfo));
	}

	NativeHandle VulkanTexture3D::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		VkImageView view = mImage.GetNativeHandle().As<AllocatedImage>()->GetView();
		return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(view));
	}

} // namespace BHive