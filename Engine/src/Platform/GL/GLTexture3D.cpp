#include "GLTexture3D.h"
#include "gfx/TextureUtils.h"
#include <glad/glad.h>
#include "threading/Threading.h"

namespace BHive
{
	GLTexture3D::GLTexture3D(uint32_t width, uint32_t height, uint32_t depth, const FTextureSpecification &specification)
		: mWidth(width), mHeight(height), mDepth(depth), mSpecification(specification)
	{
		BEGIN_THREAD_DISPATCH(=)
		GLenum target = GetTextureTarget(specification.Type, 1);

		glCreateTextures(target, 1, &mTextureID);

		glTextureStorage3D(mTextureID, 1, GetGLInternalFormat(specification.Format), width, height, depth);

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(specification.MinFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(specification.MagFilter));

		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(specification.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(specification.WrapMode));
		// glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_R, GetGLWrapMode(specification.WrapMode));

		if (mSpecification.WrapMode == EWrapMode::CLAMP_TO_BORDER)
		{
			glTextureParameterfv(mTextureID, GL_TEXTURE_BORDER_COLOR, mSpecification.BorderColor);
		}

		if (IsDepthFormat(specification.Format))
		{
			if (specification.CompareMode.has_value())
			{
				glTextureParameteri(mTextureID, GL_TEXTURE_COMPARE_MODE, GetTextureCompareMode(specification.CompareMode.value()));
			}

			if (specification.CompareFunc.has_value())
			{
				glTextureParameteri(mTextureID, GL_TEXTURE_COMPARE_FUNC, GetTextureCompareFunc(specification.CompareFunc.value()));
			}
		}

		if (mSpecification.Mips)
		{
			glGenerateTextureMipmap(mTextureID);
		}

		END_THREAD_DISPATCH()
	}

	GLTexture3D::~GLTexture3D()
	{
		BEGIN_THREAD_DISPATCH(=)
		glDeleteTextures(1, &mTextureID);
		END_THREAD_DISPATCH()
	}

	void GLTexture3D::Bind(uint32_t slot) const
	{
		BEGIN_THREAD_DISPATCH(=)
		glBindTextureUnit(slot, mTextureID);
		END_THREAD_DISPATCH()
	}

	void GLTexture3D::UnBind(uint32_t slot) const
	{
		BEGIN_THREAD_DISPATCH(=)
		glBindTextureUnit(slot, 0);
		END_THREAD_DISPATCH()
	}

	void GLTexture3D::BindAsImage(uint32_t unit, uint32_t access, uint32_t level) const
	{
		BEGIN_THREAD_DISPATCH(=)
		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, GetGLInternalFormat(mSpecification.Format));
		END_THREAD_DISPATCH()
	}

	void GLTexture3D::GenerateMipMaps() const
	{
		BEGIN_THREAD_DISPATCH(=)
		glGenerateTextureMipmap(mTextureID);
		END_THREAD_DISPATCH()
	}

	void GLTexture3D::SetData(const void *data, uint64_t size, uint32_t offsetX, uint32_t offsetY)
	{
		BEGIN_THREAD_DISPATCH(=)
		glTextureSubImage3D(mTextureID, 0, offsetX, offsetY, 0, mWidth, mHeight, mDepth, GetGLFormat(mSpecification.Format),
							GetGLType(mSpecification.Format), data);
		END_THREAD_DISPATCH()
	}
}