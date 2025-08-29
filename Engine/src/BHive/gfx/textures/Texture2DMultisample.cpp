#include "Texture2DMultisample.h"
#include <glad/glad.h>

namespace BHive
{
	Texture2DMultisample::Texture2DMultisample(uint32_t width, uint32_t height, uint32_t samples, const FTextureCreateInfo &create_info)
		: mWidth(width),
		  mHeight(height),
		  mSamples(samples),
		  mCreateInfo(create_info),
		  mInfo(create_info)
	{
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &mTextureID);
		glTextureStorage2DMultisample(mTextureID, samples, mInfo.InternalFormat, width, height, false);
	}

	Texture2DMultisample::~Texture2DMultisample()
	{
		glDeleteTextures(1, &mTextureID);
	}

	void Texture2DMultisample::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, mTextureID);
	}

	void Texture2DMultisample::UnBind(uint32_t slot) const
	{
		glBindTextureUnit(slot, 0);
	}

	void Texture2DMultisample::GenerateMipMaps() const
	{
	}
} // namespace BHive