#include "gfx/Framebuffer.h"
#include "gfx/utils/texture/TextureUtils.h"
#include "TextureCube.h"
#include <glad/glad.h>

namespace BHive
{
	TextureCube::TextureCube(uint32_t size, const FTextureCreateInfo &create_info)
		: mSize(size),
		  mCreateInfo(create_info),
		  mInfo(create_info)
	{

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &mTextureID);

		glTextureStorage2D(mTextureID, mInfo.Levels, mInfo.InternalFormat, size, size);

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, mInfo.FilterModes[0]);
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, mInfo.FilterModes[1]);

		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_R, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);

		if (mInfo.WrapMode == GL_CLAMP_TO_BORDER)
		{
			glTextureParameterfv(mTextureID, GL_TEXTURE_BORDER_COLOR, mInfo.BorderColor);
		}

		for (unsigned i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInfo.InternalFormat, size, size, 0, mInfo.Format, mInfo.Type, NULL);
		}

		if (mInfo.Levels > 1 || mInfo.GenerateMipMaps)
		{
			glGenerateTextureMipmap(mTextureID);
		}
	}

	TextureCube::~TextureCube()
	{
		glDeleteTextures(1, &mTextureID);
	}

	void TextureCube::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void TextureCube::UnBind(uint32_t slot) const
	{

		glBindTextureUnit(slot, 0);
	}

} // namespace BHive