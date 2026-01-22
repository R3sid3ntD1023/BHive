#include "gfx/utils/texture/TextureUtils.h"
#include "Platform/Vulkan/VulkanUtils.h"
#include "VulkanTexture2D.h"

namespace BHive
{
	VulkanTexture2D::VulkanTexture2D()
		: mDevice(VulkanCore::GetLogicalDevice())
	{
	}

	VulkanTexture2D::VulkanTexture2D(uint32_t w, uint32_t h, const FTextureCreateInfo &info, const void *buffer, size_t size)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mWidth(w),
		  mHeight(h),
		  mCreateInfo(info),
		  mInfo(info)
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

		AllocatedVulkanBuffer stagingBuffer;

		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer);

		stagingBuffer.SetData(data, size, 0);

		auto &image = mVulkanTexture.Image;
		VulkanUtils::TransitionImageLayout(image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		VulkanUtils::CopyBufferToImage(stagingBuffer, mVulkanTexture, mWidth, mHeight);
		VulkanUtils::TransitionImageLayout(image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
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
		// glGetTextureSubImage(mTextureID, 0, texture.x, texture.y, texture.z, texture.width, texture.height, texture.depth, mInfo.Format, mInfo.Type, size, data);
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

		VulkanUtils::CreateImage(
			mWidth, mHeight, 1, vk::ImageType::e2D, format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal, mVulkanTexture);

		VulkanUtils::CreateImageView(mVulkanTexture, vk::ImageViewType::e2D, format);

		vk::SamplerCreateInfo sampler_info({}, min_filter, mag_filter, vk::SamplerMipmapMode::eLinear, wrap_mode, wrap_mode, wrap_mode, 0, 0, 1, compare_enabled, compare_operation);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		VulkanUtils::CreateImageSampler(mVulkanTexture, sampler_info);

		mDescriptorInfo = VulkanUtils::CreateDescriptorImageInfo(mVulkanTexture, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void VulkanTexture2D::Release()
	{
		mBuffer.Release();
	}

	void VulkanTexture2D::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mWidth, mHeight, mCreateInfo, mBuffer);
	}

	void VulkanTexture2D::Load(cereal::BinaryInputArchive &ar)
	{

		Asset::Load(ar);

		ar(mWidth, mHeight, mCreateInfo, mBuffer);
		mInfo = mCreateInfo;

		if (mBuffer)
		{
			Initialize();
			SetData(mBuffer);
		}
	}

} // namespace BHive