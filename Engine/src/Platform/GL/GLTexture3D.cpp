#include "GLTexture3D.h"
#include "gfx/TextureUtils.h"
#include <glad/glad.h>

namespace BHive
{
	GLTexture3D::GLTexture3D(uint32_t width, uint32_t height, uint32_t depth, const FTextureSpecification &specification)
		: mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mSpecification(specification)
	{

		GLenum target = GetTextureTarget(specification.mType, 1);

		glCreateTextures(target, 1, &mTextureID);

		glTextureStorage3D(mTextureID, 1, GetGLInternalFormat(specification.mFormat), width, height, depth);

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(specification.mMinFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(specification.mMagFilter));

		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(specification.mWrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(specification.mWrapMode));
		// glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_R, GetGLWrapMode(specification.WrapMode));

		if (mSpecification.mWrapMode == EWrapMode::CLAMP_TO_BORDER)
		{
			glTextureParameterfv(mTextureID, GL_TEXTURE_BORDER_COLOR, mSpecification.mBorderColor);
		}

		if (IsDepthFormat(specification.mFormat))
		{
			if (specification.mCompareMode.has_value())
			{
				glTextureParameteri(mTextureID, GL_TEXTURE_COMPARE_MODE, GetTextureCompareMode(specification.mCompareMode.value()));
			}

			if (specification.mCompareFunc.has_value())
			{
				glTextureParameteri(mTextureID, GL_TEXTURE_COMPARE_FUNC, GetTextureCompareFunc(specification.mCompareFunc.value()));
			}
		}

		if (mSpecification.mLevels > 1)
		{
			glGenerateTextureMipmap(mTextureID);
		}

		mHandle = glGetTextureHandleARB(mTextureID);
		glMakeTextureHandleResidentARB(mHandle);
	}

	GLTexture3D::~GLTexture3D()
	{
		glMakeTextureHandleNonResidentARB(mHandle);
		glDeleteTextures(1, &mTextureID);
	}

	void GLTexture3D::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void GLTexture3D::UnBind(uint32_t slot) const
	{

		glBindTextureUnit(slot, 0);
	}

	void GLTexture3D::BindAsImage(uint32_t unit, uint32_t access, uint32_t level) const
	{
		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, GetGLInternalFormat(mSpecification.mFormat));
	}

	void GLTexture3D::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

	void GLTexture3D::SetData(const void *data, uint64_t size, uint32_t offsetX, uint32_t offsetY)
	{

		glTextureSubImage3D(
			mTextureID, 0, offsetX, offsetY, 0, mWidth, mHeight, mDepth, GetGLFormat(mSpecification.mFormat), GetGLType(mSpecification.mFormat),
			data);
	}
} // namespace BHive