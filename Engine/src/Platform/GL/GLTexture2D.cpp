#include "GLTexture2D.h"
#include "gfx/TextureUtils.h"
#include <glad/glad.h>
#include "core/threading/Threading.h"

namespace BHive
{
	GLTexture2D::GLTexture2D(uint32_t width, uint32_t height, const FTextureSpecification &specification, uint32_t samples)
		: mWidth(width),
		  mHeight(height),
		  mSpecification(specification),
		  mSamples(samples)
	{

		Initialize();
	}

	GLTexture2D::GLTexture2D(const void *data, uint32_t width, uint32_t height, const FTextureSpecification &specification)
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

	// void GLTexture2D::SetWrapMode(EWrapMode mode)
	// {
	// 	mSpecification.mWrapMode = mode;
	// 	// glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_S, GetGLWrapMode(mode));
	// 	// glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_T, GetGLWrapMode(mode));
	// }

	// void GLTexture2D::SetMinFilter(EMinFilter mode)
	// {
	// 	mSpecification.mMinFilter = mode;
	// 	// glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(mode));
	// }

	// void GLTexture2D::SetMagFilter(EMagFilter mode)
	// {
	// 	mSpecification.mMagFilter = mode;
	// 	// glSamplerParameteri(mSamplerID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(mode));
	// }

	void GLTexture2D::Bind(uint32_t slot) const
	{
		// glBindSampler(slot, mSamplerID);
		glBindTextureUnit(slot, mTextureID);
	}

	void GLTexture2D::UnBind(uint32_t slot) const
	{
		// glBindSampler(slot, 0);
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

		glTextureSubImage2D(
			mTextureID, 0, offsetX, offsetY, mWidth, mHeight, GetGLFormat(mSpecification.mFormat), GetGLType(mSpecification.mFormat), data);
	}

	void GLTexture2D::Load(cereal::BinaryInputArchive &ar)
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

	void GLTexture2D::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mWidth, mHeight, mSpecification);
		ar(mBuffer);
	}

	void GLTexture2D::Initialize()
	{
		GLenum target = GetTextureTarget(mSpecification.mType, mSamples);

		glCreateTextures(target, 1, &mTextureID);
		// ;

		switch (target)
		{
		case GL_TEXTURE_2D_MULTISAMPLE:
		{

			glTextureStorage2DMultisample(mTextureID, mSamples, GetGLInternalFormat(mSpecification.mFormat), mWidth, mHeight, GL_FALSE);
			break;
		}

		case GL_TEXTURE_2D:
		{
			// glCreateSamplers(1, &mSamplerID);
			glTextureStorage2D(mTextureID, mSpecification.mLevels, GetGLInternalFormat(mSpecification.mFormat), mWidth, mHeight);

			// glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(mSpecification.mMinFilter));
			// glSamplerParameteri(mSamplerID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(mSpecification.mMagFilter));
			// glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_S, GetGLWrapMode(mSpecification.mWrapMode));
			// glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_T, GetGLWrapMode(mSpecification.mWrapMode));

			glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(mSpecification.mMinFilter));
			glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(mSpecification.mMagFilter));
			glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(mSpecification.mWrapMode));
			glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(mSpecification.mWrapMode));

			if (mSpecification.mLevels > 1)
			{
				glGenerateTextureMipmap(mTextureID);
			}

			break;
		}
		default:
			break;
		};

		// mHandle = glGetTextureHandleNV(mTextureID);
		// glMakeTextureHandleResidentNV(mHandle);

		// // mSamplerHandle = glGetTextureSamplerHandleNV(mTextureID, mSamplerID);

		// if (mSpecification.mAccess)
		// {
		// 	mImageHandle = glGetImageHandleNV(mTextureID, 0, GL_FALSE, 0, GetGLInternalFormat(mSpecification.mFormat));
		// 	glMakeImageHandleResidentNV(mImageHandle, GetGLAccess(mSpecification.mAccess.value()));
		// }
	}

	void GLTexture2D::Release()
	{

		// BEGIN_THREAD_DISPATCH(=)

		// glMakeTextureHandleNonResidentARB(mHandle);
		glDeleteTextures(1, &mTextureID);
		// glDeleteSamplers(1, &mSamplerID);

		// END_THREAD_DISPATCH()

		mBuffer.Release();
	}
} // namespace BHive