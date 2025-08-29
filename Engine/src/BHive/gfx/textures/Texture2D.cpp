#include "gfx/utils/texture/TextureUtils.h"
#include "Texture2D.h"
#include <glad/glad.h>

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

		glBindTextureUnit(slot, mTextureID);
	}

	void Texture2D::UnBind(uint32_t slot) const
	{
		glBindTextureUnit(slot, 0);
	}

	void Texture2D::BindAsImage(uint32_t unit, EImageAccess image_access, uint32_t level) const
	{
		auto access = TextureUtils::GetAPIImageAccess(image_access);
		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, mInfo.InternalFormat);
	}

	void Texture2D::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

	void Texture2D::SetInfo(const FTextureCreateInfo &info)
	{
		mCreateInfo.MinFilter = info.MinFilter;
		mCreateInfo.MagFilter = info.MagFilter;
		mCreateInfo.WrapMode = info.WrapMode;
		mInfo = mCreateInfo;

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, mInfo.FilterModes[0]);
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, mInfo.FilterModes[1]);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, mInfo.WrapMode);
	}

	void Texture2D::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
		glTextureSubImage2D(mTextureID, 0, offsetX, offsetY, mWidth, mHeight, mInfo.Format, mInfo.Type, data);
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
		glGetTextureSubImage(mTextureID, 0, texture.x, texture.y, texture.z, texture.width, texture.height, texture.depth, mInfo.Format, mInfo.Type, size, data);
	}

	void Texture2D::Initialize()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);

		glTextureStorage2D(mTextureID, mInfo.Levels, mInfo.InternalFormat, mWidth, mHeight);

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, mInfo.FilterModes[0]);
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, mInfo.FilterModes[1]);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, mInfo.WrapMode);

		glGenerateTextureMipmap(mTextureID);
	}

	void Texture2D::Release()
	{
		glDeleteTextures(1, &mTextureID);

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