#include "GLTexture2D.h"
#include "gfx/TextureUtils.h"
#include <glad/glad.h>
#include "threading/Threading.h"

namespace BHive
{
	GLTexture2D::GLTexture2D(uint32_t width, uint32_t height,
							 const FTextureSpecification &specification, uint32_t samples)
		: mWidth(width),
		  mHeight(height),
		  mSpecification(specification),
		  mSamples(samples)
	{

		Initialize();
	}

	GLTexture2D::GLTexture2D(const void *data, uint32_t width, uint32_t height,
							 const FTextureSpecification &specification)
		: mWidth(width),
		  mHeight(height),
		  mSpecification(specification),
		  mSamples(0)
	{

		Initialize();

		if (data)
		{
			SetData(data, width * height * specification.mChannels);
		}
	}

	GLTexture2D::~GLTexture2D()
	{
		Release();
	}

	void GLTexture2D::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void GLTexture2D::UnBind(uint32_t slot) const
	{

		glBindTextureUnit(slot, 0);
	}

	void GLTexture2D::BindAsImage(uint32_t unit, uint32_t access, uint32_t level) const
	{
		auto format = GetGLInternalFormat(mSpecification.mFormat);

		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, format);
	}

	void GLTexture2D::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

	void GLTexture2D::SetData(const void *data, uint64_t size, uint32_t offsetX, uint32_t offsetY)
	{
		auto bbp = mWidth * mHeight * mSpecification.mChannels;
		ASSERT(bbp == size);

		mBuffer.Allocate(size);
		memcpy_s(mBuffer.mData, mBuffer.mSize, (uint8_t *)data, size);

		glTextureSubImage2D(mTextureID, 0, offsetX, offsetY, mWidth, mHeight,
							GetGLFormat(mSpecification.mFormat), GetGLType(mSpecification.mFormat),
							data);
	}

	void GLTexture2D::Load(cereal::JSONInputArchive &ar)
	{
		Asset::Load(ar);
		ar(MAKE_NVP("Width", mWidth), MAKE_NVP("Height", mHeight),
		   MAKE_NVP("Specification", mSpecification));
		ar(MAKE_NVP("Data", mBuffer));

		Initialize();

		if (mBuffer)
		{
			SetData(mBuffer.mData, mBuffer.mSize);
		}
	}

	void GLTexture2D::Save(cereal::JSONOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(MAKE_NVP("Width", mWidth), MAKE_NVP("Height", mHeight),
		   MAKE_NVP("Specification", mSpecification));
		ar(MAKE_NVP("Data", mBuffer));

	}

	void GLTexture2D::Initialize()
	{
		GLenum target = GetTextureTarget(mSpecification.mType, mSamples);

		glCreateTextures(target, 1, &mTextureID);

		switch (target)
		{
		case GL_TEXTURE_2D_MULTISAMPLE:
		{

			glTextureStorage2DMultisample(mTextureID, mSamples,
										  GetGLInternalFormat(mSpecification.mFormat), mWidth,
										  mHeight, GL_FALSE);
			break;
		}

		case GL_TEXTURE_2D:
		{
			glTextureStorage2D(mTextureID, mSpecification.mLevels,
							   GetGLInternalFormat(mSpecification.mFormat), mWidth, mHeight);

			glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER,
								GetGLFilterMode(mSpecification.mMinFilter));
			glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER,
								GetGLFilterMode(mSpecification.mMagFilter));

			glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S,
								GetGLWrapMode(mSpecification.mWrapMode));
			glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T,
								GetGLWrapMode(mSpecification.mWrapMode));

			if (mSpecification.mMips)
			{
				glGenerateTextureMipmap(mTextureID);
			}

			break;
		}
		default:
			break;
		};
	}

	void GLTexture2D::Release()
	{
		auto id = mTextureID;
		BEGIN_THREAD_DISPATCH(=)
		glDeleteTextures(1, &id);
		END_THREAD_DISPATCH()

		mBuffer.Release();
	}
} // namespace BHive