#include "Platform/Vulkan/VulkanUtils.h"
#include "VulkanTexture2D.h"
#include "Platform/Vulkan/VulkanConverters.h"

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
		LOG_TRACE("VulkanTexture2D Descructor Called")

		mBuffer.Release();
	}

	void VulkanTexture2D::SetInfo(const FTextureCreateInfo &info)
	{
		mCreateInfo = info;
	}

	void VulkanTexture2D::SetData(const FTextureUploadInfo &info)
	{
		size_t size = mSize.x * mSize.y * GetFormatLayout(mCreateInfo.Format);

		glm::uvec3 extents = glm::compMul(info.Extent) == 0 ? glm::uvec3{mSize, 1} : info.Extent;
		ImageCopyRegion region{.BaseArrayLayer = info.ArrayLayer, .LayerCount = info.LayerCount, .Offset = info.Offset, .Extents = extents};
		ImageSubresource sub{.MipLevel = info.MipLevel, .BaseArrayLayer = info.ArrayLayer, .LayerCount = info.LayerCount};
		mImage.Upload(info.Data, size, region, sub);
	}

	Ref<Texture2D> VulkanTexture2D::CreateSubTexture(const FSubTexture &texture)
	{
		auto c = GetFormatLayout(mCreateInfo.Format);
		size_t size = texture.Size.x * texture.Size.y * c;

		Buffer pixels(size);
		GetSubImage(texture, size, &pixels[0]);

		return Texture2D::Create(texture.Size, mCreateInfo, pixels);
	}

	void VulkanTexture2D::GetSubImage(const FSubTexture &texture, size_t size, uint8_t *data) const
	{
		
	}

	void VulkanTexture2D::Initialize()
	{
		auto api_info = Vulkan::Convert(mCreateInfo);

		vk::SamplerCreateInfo sampler_info({}, api_info.MinFilter, api_info.MagFilter, 
			vk::SamplerMipmapMode::eLinear, api_info.WrapMode, api_info.WrapMode, api_info.WrapMode, 0, 0, 1, api_info.CompareEnabled, api_info.CompareOp);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		mImage.Create(mSize.x, mSize.y, 1, 1, vk::ImageType::e2D, vk::ImageViewType::e2D, api_info.Format, api_info.Usage, api_info.Aspect, sampler_info);
	}

	void VulkanTexture2D::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mImage.GetWidth(), mImage.GetHeight(), mCreateInfo, mBuffer);
	}

	void VulkanTexture2D::Load(cereal::BinaryInputArchive &ar)
	{
		uint32_t width = 0, height = 0;
		Asset::Load(ar);

		ar(width, height, mCreateInfo, mBuffer);

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