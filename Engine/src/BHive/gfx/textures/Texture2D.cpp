#include "gfx/utils/texture/TextureUtils.h"
#include "Texture2D.h"
#include "gfx/VulkanUtils.h"

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

		// glBindTextureUnit(slot, mTextureID);
	}

	void Texture2D::UnBind(uint32_t slot) const
	{
		// glBindTextureUnit(slot, 0);
	}

	void Texture2D::SetInfo(const FTextureCreateInfo &info)
	{
		mCreateInfo.MinFilter = info.MinFilter;
		mCreateInfo.MagFilter = info.MagFilter;
		mCreateInfo.WrapMode = info.WrapMode;
		mInfo = mCreateInfo;

		/*glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, mInfo.FilterModes[0]);
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, mInfo.FilterModes[1]);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, mInfo.WrapMode);

		if (mInfo.Levels > 1 || mInfo.GenerateMipMaps)
		{
			glGenerateTextureMipmap(mTextureID);
		}*/
	}

	void Texture2D::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
		/*glTextureSubImage2D(mTextureID, 0, offsetX, offsetY, mWidth, mHeight, mInfo.Format, mInfo.Type, data);

		if (mInfo.Levels > 1 || mInfo.GenerateMipMaps)
		{
			glGenerateTextureMipmap(mTextureID);
		}*/

		vk::DeviceSize size = mWidth * mHeight * mCreateInfo.Channels;

		vk::raii::Buffer stagingBuffer = nullptr;
		vk::raii::DeviceMemory stagingBufferMemory = nullptr;
		VulkanUtils::CreateBuffer(
			size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

		void *stagingData = stagingBufferMemory.mapMemory(0, size);
		memcpy(stagingData, data, size);
		stagingBufferMemory.unmapMemory();

		VulkanUtils::TransitionImageLayout(mTextureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		VulkanUtils::CopyBufferToImage(stagingBuffer, mTextureImage, mWidth, mHeight);
		VulkanUtils::TransitionImageLayout(mTextureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
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
		/*glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);

		glTextureStorage2D(mTextureID, mInfo.Levels, mInfo.InternalFormat, mWidth, mHeight);

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, mInfo.FilterModes[0]);
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, mInfo.FilterModes[1]);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, mInfo.WrapMode);*/


		auto channels = mCreateInfo.Channels;
		auto mag_filter = (vk::Filter)mInfo.FilterModes[0];
		auto min_filter = (vk::Filter)mInfo.FilterModes[1];
		auto wrap_mode = (vk::SamplerAddressMode)mInfo.WrapMode;
		auto compare_enabled = (vk::Bool32)mInfo.CompareMode;
		auto compare_operation = (vk::CompareOp)mInfo.CompareFunc;

		vk::Format format = vk::Format::eR8G8B8A8Srgb;
		VulkanUtils::CreateImage2D(
			mWidth, mHeight, format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mTextureImage,
			mTextureImageMemory);

		mTextureImageView = VulkanUtils::CreateImageView2D(mTextureImage, format);

		vk::SamplerCreateInfo sampler_info(
			{}, min_filter, mag_filter, vk::SamplerMipmapMode::eLinear,wrap_mode, wrap_mode, wrap_mode, 0, 0, 1,
			compare_enabled, compare_operation);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		mTextureSampler = VulkanUtils::CreateImageSampler(sampler_info);
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