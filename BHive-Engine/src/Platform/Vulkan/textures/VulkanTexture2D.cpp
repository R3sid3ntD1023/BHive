#include "Platform/Vulkan/VulkanUtils.h"
#include "VulkanTexture2D.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "Platform/Vulkan/VulkanBackend.h"

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
		auto format = ToVkFormat(mCreateInfo.Format);
		auto levels = mCreateInfo.MipLevels;
		auto layers = mCreateInfo.ArrayLayers;
		auto extent = vk::Extent3D(mSize.x, mSize.y, 1);
		auto usage = ToVKImageUsage(mCreateInfo.Usage);

		ImageCreateInfo create_info{};
		create_info.ImageCI = vk::ImageCreateInfo({}, vk::ImageType::e2D, format, extent, levels, layers,
			vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, 0);

		//create default view info image is set in VulkanImage
		auto aspect = ToVkAspect(mCreateInfo.Aspect);
		auto range = vk::ImageSubresourceRange(aspect, 0, levels, 0, layers);
		create_info.ViewCI = vk::ImageViewCreateInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e2D, format, {}, range);
		
		//create sampler info
		auto magFilter = ToVkFilter(mCreateInfo.MagFilter);
		auto minFilter = ToVkFilter(mCreateInfo.MinFilter);
		auto addressMode = ToVkWrap(mCreateInfo.WrapMode);
		auto compare_enabled = mCreateInfo.CompareOp.has_value();
		auto compare_op =  compare_enabled ? ToVkCompare(mCreateInfo.CompareOp.value()) : vk::CompareOp::eAlways;

		create_info.SamplerCI =
			vk::SamplerCreateInfo({}, magFilter, minFilter, vk::SamplerMipmapMode::eLinear, addressMode, addressMode, addressMode,  0.0f, 0u, 1.0f, compare_enabled, compare_op, 0.0f, 0.0f, vk::BorderColor::eIntOpaqueBlack, VK_FALSE);
		create_info.DebugName = mCreateInfo.DebugName;
		create_info.BytesPerPixel = GetBytesPerPixel(mCreateInfo.Format);
		create_info.ViewTopology = EViewTopology::Mips2D;

		mImage.Initialize(create_info);
	}

	NativeHandle VulkanTexture2D::GetRenderView(uint32_t layer, uint32_t mip) const
	{
		VkImageView view = mImage.Native().GetMipView(mip);
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