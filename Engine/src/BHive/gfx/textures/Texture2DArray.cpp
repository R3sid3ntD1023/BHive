#include "gfx/utils/texture/TextureUtils.h"
#include "Texture2DArray.h"
#include <glad/glad.h>

namespace BHive
{
	Texture2DArray::Texture2DArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification)
		: mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mCreateInfo(specification),
		  mInfo(specification)
	{

		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &mTextureID);

		glTextureStorage3D(mTextureID, specification.Levels, mInfo.InternalFormat, width, height, depth);

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, mInfo.FilterModes[0]);
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, mInfo.FilterModes[1]);

		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, mInfo.WrapMode);

		if (mInfo.WrapMode == GL_CLAMP_TO_BORDER)
		{
			glTextureParameterfv(mTextureID, GL_TEXTURE_BORDER_COLOR, mInfo.BorderColor);
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

		if (mInfo.Levels > 1 || mInfo.GenerateMipMaps)
		{
			glGenerateTextureMipmap(mTextureID);
		}
	}

	Texture2DArray::~Texture2DArray()
	{
		glDeleteTextures(1, &mTextureID);
	}

	void Texture2DArray::Bind(uint32_t slot) const
	{

		glBindTextureUnit(slot, mTextureID);
	}

	void Texture2DArray::UnBind(uint32_t slot) const
	{

		glBindTextureUnit(slot, 0);
	}

	void Texture2DArray::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
		glTextureSubImage3D(mTextureID, 0, offsetX, offsetY, 0, mWidth, mHeight, mDepth, mInfo.Format, mInfo.Type, data);
		if (mInfo.Levels > 1 || mInfo.GenerateMipMaps)
		{
			glGenerateTextureMipmap(mTextureID);
		}
	}
} // namespace BHive