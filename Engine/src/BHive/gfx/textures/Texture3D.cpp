#include "Texture3D.h"
#include <glad/glad.h>

namespace BHive
{

	Texture3D::Texture3D(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &create_info, const void *data)
		: mWidth(width),
		  mHeight(height),
		  mDepth(depth),
		  mInfo(create_info),
		  mCreateInfo(create_info)
	{
		glCreateTextures(GL_TEXTURE_3D, 1, &mTextureID);

		glTextureStorage3D(mTextureID, mInfo.Levels, mInfo.InternalFormat, mWidth, mHeight, mDepth);

		glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, mInfo.FilterModes[0]);
		glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, mInfo.FilterModes[1]);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_R, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, mInfo.WrapMode);
		glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, mInfo.WrapMode);

		if (data)
		{
			glTextureSubImage3D(mTextureID, 0, 0, 0, 0, mWidth, mHeight, mDepth, mInfo.Format, mInfo.Type, data);
		}

		if (mInfo.GenerateMipMaps)
			glGenerateTextureMipmap(mTextureID);
	}

	Texture3D::~Texture3D()
	{
		glDeleteTextures(1, &mTextureID);
	}

	void Texture3D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, mTextureID);
	}

	void Texture3D::UnBind(uint32_t slot) const
	{
		glBindTextureUnit(slot, 0);
	}

	void Texture3D::SetData(const void *data, uint32_t offsetX, uint32_t offsetY)
	{
	}
} // namespace BHive