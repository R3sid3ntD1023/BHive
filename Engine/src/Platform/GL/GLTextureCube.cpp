#include "GLTextureCube.h"
#include "gfx/TextureUtils.h"
#include "gfx/Framebuffer.h"
#include <glad/glad.h>
#include "threading/Threading.h"

namespace BHive
{
	GLTextureCube::GLTextureCube(uint32_t size, const FTextureSpecification &spec)
		: mSize(size), mSpecification(spec)
	{

		GLenum target = GetTextureTarget(spec.Type, 1);
		glCreateTextures(target, 1, &mTextureID);

		unsigned levels = spec.Mips ? spec.Levels : 1;

		glTextureStorage2D(mTextureID, levels, GetGLInternalFormat(spec.Format), size, size);
		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GetGLFilterMode(spec.MinFilter));
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GetGLFilterMode(spec.MagFilter));

		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GetGLWrapMode(spec.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GetGLWrapMode(spec.WrapMode));
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_R, GetGLWrapMode(spec.WrapMode));
		// glTextureParameteri(mTextureID, GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);

		if (spec.WrapMode == EWrapMode::CLAMP_TO_BORDER)
		{
			glTextureParameterfv(mTextureID, GL_TEXTURE_BORDER_COLOR, spec.BorderColor);
		}

		for (unsigned i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GetGLInternalFormat(spec.Format), size, size,
						 0, GetGLFormat(spec.Format), GetGLType(spec.Format), NULL);
		}

		if (spec.Mips)
		{
			glGenerateTextureMipmap(mTextureID);
		}
	}

	GLTextureCube::~GLTextureCube()
	{

		glDeleteTextures(1, &mTextureID);
	}

	void GLTextureCube::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void GLTextureCube::BindAsImage(uint32_t unit, uint32_t access, uint32_t level) const
	{

		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, GetGLInternalFormat(mSpecification.Format));
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
}