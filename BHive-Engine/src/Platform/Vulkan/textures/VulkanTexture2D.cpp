#include "Platform/Vulkan/VulkanUtils.h"
#include "VulkanTexture2D.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "Platform/Vulkan/GPUComponents.h"

namespace BHive
{
	VulkanTexture2D::VulkanTexture2D()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
	}

	VulkanTexture2D::VulkanTexture2D(const glm::uvec2& size, const FTextureCreateInfo &createInfo, const Buffer& data)
		: mDevice(VulkanBackend::GetLogicalDevice()),	
		  mSize(size),
		  mBuffer(data),
		  mCreateInfo(createInfo)
	{

		Initialize();

		if (data)
		{
			FTextureUploadInfo info{
				.Data = data.As<uint8_t>(),
			};
			SetData(info);
		}
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		mBuffer.Release();
	}

	void VulkanTexture2D::SetInfo(const FTextureCreateInfo &info)
	{
		mCreateInfo = info;
	}

	void VulkanTexture2D::SetData(const FTextureUploadInfo &info)
	{
		auto size = mBuffer.GetSize();

		glm::uvec3 extents = glm::compMul(info.Extent) == 0 ? glm::uvec3{mSize, 1} : info.Extent;
		ImageCopyRegion region{.BaseArrayLayer = info.ArrayLayer, .LayerCount = info.LayerCount, .Offset = info.Offset, .Extents = extents};
		ImageSubresource sub{.MipLevel = info.MipLevel, .BaseArrayLayer = info.ArrayLayer, .LayerCount = info.LayerCount};
		mImage.Upload(info.Data, size, region, sub);
	}

	Ref<Texture2D> VulkanTexture2D::CreateSubTexture(const FSubTexture &texture)
	{
		auto size = mBuffer.GetSize();

		Buffer pixels(size);
		GetSubImage(texture, size, &pixels[0]);

		return Texture2D::Create(texture.Size, mCreateInfo, pixels);
	}

	void VulkanTexture2D::GetSubImage(const FSubTexture &texture, size_t size, uint8_t *data) const
	{
		
	}

	void VulkanTexture2D::Initialize()
	{
		ImageCreateInfo create_info{};
		create_info.CreateFlags = {};
		create_info.Width = mSize.x;
		create_info.Height = mSize.y;
		create_info.Depth = 1;
		create_info.Type = vk::ImageType::e2D;
		create_info.ViewType = vk::ImageViewType::e2D;
		create_info.CreateInfo = Convert(mCreateInfo);
		mImage.Initialize(create_info);
	}

	NativeHandle VulkanTexture2D::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		auto image = mImage.GetNativeHandle().As<GPUImage>();
		VkImageView view = image->GetComponent<MipViewComponent>()->Get(layer, mip);
		return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(view));
	}

	void VulkanTexture2D::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mSize, mCreateInfo, mBuffer);
	}

	void VulkanTexture2D::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);

		ar(mSize, mCreateInfo, mBuffer);

		Initialize();

		if (mBuffer)
		{	
			FTextureUploadInfo info{
				.Data = mBuffer.As<uint8_t>(),
			};
			SetData(info);
		}
	}

} // namespace BHive