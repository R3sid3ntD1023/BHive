#include "GLTextureCube.h"
#include "gfx/TextureUtils.h"
#include "gfx/Framebuffer.h"
#include <glad/glad.h>

namespace BHive
{
	GLTextureCube::GLTextureCube(uint32_t size, const FTextureSpecification &spec)
		: mSize(size),
		  mSpecification(spec)
	{

		GLenum target = GetTextureTarget(spec.mType, 1);
		glCreateTextures(target, 1, &mTextureID);

		glTextureStorage2D(mTextureID, spec.mLevels, GetGLInternalFormat(spec.mFormat), size, size);
		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(spec.mMinFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(spec.mMagFilter));

		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(spec.mWrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(spec.mWrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_R, GetGLWrapMode(spec.mWrapMode));
		// glTextureParameteri(mTextureID, GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);

		if (spec.mWrapMode == EWrapMode::CLAMP_TO_BORDER)
		{
			glTextureParameterfv(mTextureID, GL_TEXTURE_BORDER_COLOR, spec.mBorderColor);
		}

		for (unsigned i = 0; i < 6; i++)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GetGLInternalFormat(spec.mFormat), size, size, 0, GetGLFormat(spec.mFormat),
				GetGLType(spec.mFormat), NULL);
		}

		if (spec.mLevels > 1)
		{
			glGenerateTextureMipmap(mTextureID);
		}

		// mHandle = glGetTextureHandleARB(mTextureID);
		// glMakeTextureHandleResidentARB(mHandle);
	}

	GLTextureCube::~GLTextureCube()
	{
		// glMakeTextureHandleNonResidentARB(mHandle);
		glDeleteTextures(1, &mTextureID);
	}

	void GLTextureCube::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void GLTextureCube::BindAsImage(uint32_t unit, uint32_t access, uint32_t level) const
	{
		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, GetGLInternalFormat(mSpecification.mFormat));
	}

	void GLTextureCube::UnBind(uint32_t slot) const
	{

		glBindTextureUnit(slot, 0);
	}

	void GLTextureCube::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

	void GLTextureCube::AttachToFramebuffer(const Ref<Framebuffer> &framebuffer, uint32_t attachment, uint32_t level)
	{

		glNamedFramebufferTexture(*framebuffer, attachment, mTextureID, level);
	}
} // namespace BHive