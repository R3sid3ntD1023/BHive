#include "GLTexture2D.h"
#include "gfx/TextureUtils.h"
#include <glad/glad.h>
#include "threading/Threading.h"

namespace BHive
{
	GLTexture2D::GLTexture2D(uint32_t width, uint32_t height, const FTextureSpecification &specification, uint32_t samples)
		: mWidth(width), mHeight(height), mSpecification(specification), mSamples(samples)
	{

		Initialize();

		/*mResourceHandle = glGetTextureHandleARB(mTextureID);
		glMakeTextureHandleResidentARB(mResourceHandle);*/
	}

	GLTexture2D::GLTexture2D(const void *data, uint32_t width, uint32_t height, const FTextureSpecification &specification)
		: mWidth(width), mHeight(height), mSpecification(specification)
	{

		unsigned levels = specification.Mips ? specification.Levels : 1;
		glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);
		glTextureStorage2D(mTextureID, levels, GetGLInternalFormat(specification.Format), width, height);

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(specification.MinFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(specification.MagFilter));

		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(specification.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(specification.WrapMode));

		if (mSpecification.Mips)
		{
			glGenerateTextureMipmap(mTextureID);
		}

		if (data)
		{
			SetData(data, width * height * mSpecification.Channels);
		}

		/*mResourceHandle = glGetTextureHandleARB(mTextureID);
		glMakeTextureHandleResidentARB(mResourceHandle);*/
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
		auto format = GetGLInternalFormat(mSpecification.Format);

		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, format);
	}

	void GLTexture2D::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

	void GLTexture2D::SetData(const void *data, uint64_t size, uint32_t offsetX, uint32_t offsetY)
	{
		auto bbp = mWidth * mHeight * mSpecification.Channels;
		ASSERT(bbp == size);

		glTextureSubImage2D(mTextureID, 0, offsetX, offsetY, mWidth, mHeight, GetGLFormat(mSpecification.Format),
							GetGLType(mSpecification.Format), data);
	}

	void GLTexture2D::Initialize()
	{

		GLenum target = GetTextureTarget(mSpecification.Type, mSamples);

		glCreateTextures(target, 1, &mTextureID);

		switch (target)
		{
		case GL_TEXTURE_2D_MULTISAMPLE:
		{

			glTextureStorage2DMultisample(mTextureID, mSamples, GetGLInternalFormat(mSpecification.Format), mWidth, mHeight, GL_FALSE);
			break;
		}

		case GL_TEXTURE_2D:
		{
			glTextureStorage2D(mTextureID, 1, GetGLInternalFormat(mSpecification.Format), mWidth, mHeight);

			glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(mSpecification.MinFilter));
			glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(mSpecification.MagFilter));

			glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(mSpecification.WrapMode));
			glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(mSpecification.WrapMode));

			if (mSpecification.WrapMode == EWrapMode::CLAMP_TO_BORDER)
			{
				glTextureParameterfv(mTextureID, GL_TEXTURE_BORDER_COLOR, mSpecification.BorderColor);
			}

			if (mSpecification.Mips)
			{
				glGenerateTextureMipmap(mTextureID);
			}

			break;
		}
		default:
			break;
		}

		;
	}

	void GLTexture2D::Release()
	{

		glDeleteTextures(1, &mTextureID);
	}
}