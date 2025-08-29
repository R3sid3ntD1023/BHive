#include "gfx/utils/texture/TextureUtils.h"
#include "TextureCubeArray.h"
#include <glad/glad.h>

namespace BHive
{
	TextureCubeArray::TextureCubeArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &spec)
		: mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mCreateInfo(spec),
		  mInfo(spec)
	{

		glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &mTextureID);

		glTextureStorage3D(mTextureID, spec.Levels, mInfo.InternalFormat, width, height, depth);

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, mInfo.FilterModes[0]);
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, mInfo.FilterModes[1]);

		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_R, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, mInfo.WrapMode);
		// glTextureParameteri(mTextureID, GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);

		if (mInfo.WrapMode == GL_CLAMP_TO_BORDER)
		{
			glTextureParameterfv(mTextureID, GL_TEXTURE_BORDER_COLOR, spec.BorderColor);
		}

		if (mInfo.IsDepth)
		{
			if (mInfo.CompareMode)
			{
				glTextureParameteri(mTextureID, GL_TEXTURE_COMPARE_MODE, mInfo.CompareMode);
			}

			if (mInfo.CompareFunc)
			{
				glTextureParameteri(mTextureID, GL_TEXTURE_COMPARE_FUNC, mInfo.CompareFunc);
			}
		}

		if (mInfo.Levels > 1)
		{
			glGenerateTextureMipmap(mTextureID);
		}
	}

	TextureCubeArray::~TextureCubeArray()
	{
		glDeleteTextures(1, &mTextureID);
	}

	void TextureCubeArray::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void TextureCubeArray::UnBind(uint32_t slot) const
	{

		glBindTextureUnit(slot, 0);
	}

	void TextureCubeArray::BindAsImage(uint32_t unit, EImageAccess image_access, uint32_t level) const
	{
		auto access = TextureUtils::GetAPIImageAccess(image_access);
		glBindImageTexture(unit, mTextureID, level, GL_FALSE, 0, access, mInfo.InternalFormat);
	}

	void TextureCubeArray::GenerateMipMaps() const
	{

		glGenerateTextureMipmap(mTextureID);
	}

	void TextureCubeArray::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
		glTextureSubImage3D(mTextureID, 0, offsetX, offsetY, 0, mWidth, mHeight, mDepth, mInfo.Format, mInfo.Type, data);
	}
} // namespace BHive