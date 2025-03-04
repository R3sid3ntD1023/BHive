#include "Texture2D.h"
#include <glad/glad.h>
#include "gfx/TextureUtils.h"

namespace BHive
{
	Texture2D::Texture2D(uint32_t width, uint32_t height, const FTextureSpecification &specification)
		: mWidth(width),
		  mHeight(height),
		  mSpecification(specification)
	{

		Initialize();
	}

	Texture2D::Texture2D(const void *data, uint32_t width, uint32_t height, const FTextureSpecification &specification)
		: mWidth(width),
		  mHeight(height),
		  mSpecification(specification)
	{

		Initialize();

		if (data)
		{
			SetData(data, width * height * specification.Channels);
		}
	}

	Texture2D::~Texture2D()
	{
		Release();
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, mTextureID);
	}

	void Texture2D::UnBind(uint32_t slot) const
	{
		glBindTextureUnit(slot, 0);
	}

	void Texture2D::BindAsImage(uint32_t unit, uint32_t access, uint32_t level) const
	{
		auto format = GetGLInternalFormat(mSpecification.InternalFormat);

		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, format);
	}

	void Texture2D::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

	void Texture2D::SetData(const void *data, uint64_t size, uint32_t offsetX, uint32_t offsetY)
	{
		auto bbp = mWidth * mHeight * mSpecification.Channels;
		ASSERT(bbp == size);

		mBuffer.Allocate(size);
		memcpy_s(mBuffer.mData, mBuffer.mSize, (uint8_t *)data, size);

		glTextureSubImage2D(
			mTextureID, 0, offsetX, offsetY, mWidth, mHeight, GetGLFormat(mSpecification.InternalFormat),
			GetGLType(mSpecification.InternalFormat), data);
	}

	void Texture2D::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mWidth, mHeight, mSpecification);
		ar(mBuffer);

		Initialize();

		if (mBuffer)
		{
			SetData(mBuffer.mData, mBuffer.mSize);
		}
	}

	void Texture2D::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mWidth, mHeight, mSpecification);
		ar(mBuffer);
	}

	void Texture2D::Initialize()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);

		glTextureStorage2D(
			mTextureID, mSpecification.Levels, GetGLInternalFormat(mSpecification.InternalFormat), mWidth, mHeight);

		if (mSpecification.Levels > 1)
		{
			glGenerateTextureMipmap(mTextureID);
		}

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(mSpecification.MinFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(mSpecification.MagFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(mSpecification.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(mSpecification.WrapMode));

		glGenerateTextureMipmap(mTextureID);

		mResourceHandle = glGetTextureHandleNV(mTextureID);

		if (!glIsTextureHandleResidentNV(mResourceHandle))
			glMakeTextureHandleResidentNV(mResourceHandle);

		if (mSpecification.ImageAccess)
		{
			mImageHandle =
				glGetImageHandleNV(mTextureID, 0, GL_FALSE, 0, GetGLInternalFormat(mSpecification.InternalFormat));
			if (!glIsImageHandleResidentNV(mImageHandle))
				glMakeImageHandleResidentNV(mImageHandle, GetGLAccess(mSpecification.ImageAccess.value()));
		}
	}

	void Texture2D::Release()
	{

		if (mImageHandle && glIsImageHandleResidentNV(mImageHandle))
			glMakeImageHandleNonResidentNV(mImageHandle);

		if (glIsTextureHandleResidentNV(mResourceHandle))
			glMakeTextureHandleNonResidentNV(mResourceHandle);

		glDeleteTextures(1, &mTextureID);

		mBuffer.Release();
	}

	REFLECT(Texture2D)
	{
		BEGIN_REFLECT(Texture2D).constructor();
	}
} // namespace BHive