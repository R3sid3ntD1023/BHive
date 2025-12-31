#include "gfx/utils/texture/TextureUtils.h"
#include "gfx/VulkanUtils.h"
#include "Texture2D.h"

namespace BHive
{
	

	Texture2D::Texture2D(uint32_t w, uint32_t h, const FTextureCreateInfo &info, const void *buffer, size_t size)
		: mWidth(w),
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

	Texture2D::~Texture2D()
	{
		Release();
		mBuffer.Release();
	}

	void Texture2D::Bind(uint32_t slot) const
	{

	
	}

	void Texture2D::UnBind(uint32_t slot) const
	{
		
	}

	void Texture2D::SetInfo(const FTextureCreateInfo &info)
	{
		mCreateInfo.MinFilter = info.MinFilter;
		mCreateInfo.MagFilter = info.MagFilter;
		mCreateInfo.WrapMode = info.WrapMode;
		mInfo = mCreateInfo;
	}

	void Texture2D::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
		vk::DeviceSize size = mWidth * mHeight * mCreateInfo.Channels;

		vk::raii::Buffer stagingBuffer = nullptr;
		vk::raii::DeviceMemory stagingBufferMemory = nullptr;
		VulkanUtils::CreateBuffer(
			size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

		void *stagingData = stagingBufferMemory.mapMemory(0, size);
		memcpy(stagingData, data, size);
		stagingBufferMemory.unmapMemory();

		auto& image = mVulkanTexture.Image;
		VulkanUtils::TransitionImageLayout(image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		VulkanUtils::CopyBufferToImage(stagingBuffer, image, mWidth, mHeight);
		VulkanUtils::TransitionImageLayout(image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	Ref<Texture2D> Texture2D::CreateSubTexture(const FSubTexture &texture)
	{
		auto c = mCreateInfo.Channels;
		size_t size = texture.width * texture.height * c;

		Buffer pixels(size);
		GetSubImage(texture, size, &pixels[0]);

		return CreateRef<Texture2D>(texture.width, texture.height, mCreateInfo, pixels);
	}

	void Texture2D::GetSubImage(const FSubTexture &texture, size_t size, uint8_t *data) const
	{
		// glGetTextureSubImage(mTextureID, 0, texture.x, texture.y, texture.z, texture.width, texture.height, texture.depth, mInfo.Format, mInfo.Type, size, data);
	}

	void Texture2D::Initialize()
	{

		auto channels = mCreateInfo.Channels;
		auto mag_filter = (vk::Filter)mInfo.FilterModes[0];
		auto min_filter = (vk::Filter)mInfo.FilterModes[1];
		auto wrap_mode = (vk::SamplerAddressMode)mInfo.WrapMode;
		auto compare_enabled = (vk::Bool32)mInfo.CompareMode;
		auto compare_operation = (vk::CompareOp)mInfo.CompareFunc;
		auto format = (vk::Format)mInfo.InternalFormat;

		VulkanUtils::CreateImage2D(
			mWidth, mHeight, format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mVulkanTexture.Image,
			mVulkanTexture.Memory);

		mVulkanTexture.ImageView = VulkanUtils::CreateImageView2D(mVulkanTexture.Image, format);

		vk::SamplerCreateInfo sampler_info(
			{}, min_filter, mag_filter, vk::SamplerMipmapMode::eLinear,wrap_mode, wrap_mode, wrap_mode, 0, 0, 1,
			compare_enabled, compare_operation);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		mVulkanTexture.Sampler = VulkanUtils::CreateImageSampler(sampler_info);

		mDescriptorInfo = vk::DescriptorImageInfo(*mVulkanTexture.Sampler, *mVulkanTexture.ImageView, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void Texture2D::Release()
	{
		// glDeleteTextures(1, &mTextureID);

		mBuffer.Release();
	}

	void Texture2D::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mWidth, mHeight, mCreateInfo, mBuffer);
	}

	void Texture2D::Load(cereal::BinaryInputArchive &ar)
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

	REFLECT(Texture2D)
	{
		BEGIN_REFLECT(Texture2D) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Specification", GetInfo, SetInfo);
		rttr::type::register_wrapper_converter_for_base_classes<Ref<Texture2D>>();
	}
} // namespace BHive