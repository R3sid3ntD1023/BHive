#include "Platform/Vulkan/VulkanUtils.h"
#include "VulkanTexture2D.h"
#include "Platform/Vulkan/VulkanConverters.h"

namespace BHive
{
	VulkanTexture2D::VulkanTexture2D()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
	}

	VulkanTexture2D::VulkanTexture2D(uint32_t w, uint32_t h, const FTextureCreateInfo &info, const void *buffer, size_t size)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mCreateInfo(info),
		  mWidth(w),
		  mHeight(h)
	{

		Initialize();

		if (buffer)
		{
			mBuffer.Allocate(buffer, size);
			SetData(buffer);
		}
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		LOG_TRACE("VulkanTexture2D Descructor Called")

		mBuffer.Release();
	}

	void VulkanTexture2D::Bind(uint32_t slot) const
	{
	}

	void VulkanTexture2D::UnBind(uint32_t slot) const
	{
	}

	void VulkanTexture2D::SetInfo(const FTextureCreateInfo &info)
	{
		mCreateInfo = info;
	}

	void VulkanTexture2D::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
		vk::DeviceSize size = mWidth * mHeight * mCreateInfo.Channels;

		mImage.Upload(data, size);
	}

	Ref<Texture2D> VulkanTexture2D::CreateSubTexture(const FSubTexture &texture)
	{
		auto c = mCreateInfo.Channels;
		size_t size = texture.width * texture.height * c;

		Buffer pixels(size);
		GetSubImage(texture, size, &pixels[0]);

		return Texture2D::Create(texture.width, texture.height, mCreateInfo, pixels);
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

		mImage.Create(mWidth, mHeight, 1, vk::ImageType::e2D, vk::ImageViewType::e2D, api_info.Format, api_info.Usage, api_info.Aspect, sampler_info);
	}

	NativeHandle VulkanTexture2D::GetNativeHandle() const
	{
		return mImage.GetNativeHandle();
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

		if (mBuffer)
		{
			Initialize();
			SetData(mBuffer);
		}
	}

} // namespace BHive