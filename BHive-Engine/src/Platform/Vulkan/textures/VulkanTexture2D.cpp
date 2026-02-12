#include "gfx/utils/texture/TextureUtils.h"
#include "Platform/Vulkan/VulkanUtils.h"
#include "VulkanTexture2D.h"

namespace BHive
{
	VulkanTexture2D::VulkanTexture2D()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
	}

	VulkanTexture2D::VulkanTexture2D(uint32_t w, uint32_t h, const FTextureCreateInfo &info, const void *buffer, size_t size)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mCreateInfo(info),
		  mInfo(info),
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

		Release();
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
		mCreateInfo.MinFilter = info.MinFilter;
		mCreateInfo.MagFilter = info.MagFilter;
		mCreateInfo.WrapMode = info.WrapMode;
		mInfo = mCreateInfo;
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

		auto channels = mCreateInfo.Channels;
		auto mag_filter = (vk::Filter)mInfo.FilterModes[0];
		auto min_filter = (vk::Filter)mInfo.FilterModes[1];
		auto wrap_mode = (vk::SamplerAddressMode)mInfo.WrapMode;
		auto compare_enabled = (vk::Bool32)mInfo.CompareMode;
		auto compare_operation = (vk::CompareOp)mInfo.CompareFunc;
		auto format = (vk::Format)mInfo.InternalFormat;

		vk::SamplerCreateInfo sampler_info({}, min_filter, mag_filter, vk::SamplerMipmapMode::eLinear, wrap_mode, wrap_mode, wrap_mode, 0, 0, 1, compare_enabled, compare_operation);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		mImage.Create(mWidth, mHeight, 1, vk::ImageType::e2D, vk::ImageViewType::e2D, format, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::ImageAspectFlagBits::eColor, sampler_info);
	}

	void VulkanTexture2D::Release()
	{
		mBuffer.Release();
	}

	NativeHandle VulkanTexture2D::GetNativeHandle() const
	{
		return NativeHandle{.Ptr = &mImage.GetDescriptor()};
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
		mInfo = mCreateInfo;

		if (mBuffer)
		{
			Initialize();
			SetData(mBuffer);
		}
	}

} // namespace BHive