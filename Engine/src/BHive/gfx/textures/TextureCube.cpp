#include "TextureCube.h"
#include "gfx/TextureUtils.h"
#include "gfx/Framebuffer.h"
#include <glad/glad.h>

namespace BHive
{
	TextureCube::TextureCube(uint32_t size, const FTextureSpecification &spec)
		: mSize(size),
		  mSpecification(spec)
	{

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &mTextureID);

		glTextureStorage2D(mTextureID, spec.Levels, GetGLInternalFormat(spec.InternalFormat), size, size);

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
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GetGLInternalFormat(spec.InternalFormat), size, size, 0,
				GetGLFormat(spec.InternalFormat), GetGLType(spec.InternalFormat), NULL);
		}

		if (spec.Levels > 1)
		{
			glGenerateTextureMipmap(mTextureID);
		}

		mResourceHandle = glGetTextureHandleNV(mTextureID);

		if (!glIsTextureHandleResidentNV(mResourceHandle))
			glMakeTextureHandleResidentNV(mResourceHandle);
	}

	TextureCube::~TextureCube()
	{
		if (glIsTextureHandleResidentNV(mResourceHandle))
			glMakeTextureHandleNonResidentNV(mResourceHandle);
		glDeleteTextures(1, &mTextureID);
	}

	void TextureCube::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void TextureCube::BindAsImage(uint32_t unit, uint32_t access, uint32_t level) const
	{
		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, GetGLInternalFormat(mSpecification.InternalFormat));
	}

	void TextureCube::UnBind(uint32_t slot) const
	{

		glBindTextureUnit(slot, 0);
	}

	void TextureCube::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

} // namespace BHive