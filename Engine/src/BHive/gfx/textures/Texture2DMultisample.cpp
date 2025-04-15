#include "Texture2DMultisample.h"
#include <glad/glad.h>
#include "gfx/TextureUtils.h"

namespace BHive
{
	Texture2DMultisample::Texture2DMultisample(
		uint32_t width, uint32_t height, uint32_t samples, const FTextureSpecification &specs)
		: mWidth(width),
		  mHeight(height),
		  mSamples(samples),
		  mSpecification(specs)		  
	{
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &mTextureID);
		glTextureStorage2DMultisample(mTextureID, samples, GetGLInternalFormat(specs.InternalFormat), width, height, false);

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

	void Texture2DMultisample::BindAsImage(uint32_t unit, EImageAccess access, uint32_t level) const
	{
	}

	void Texture2DMultisample::SetData(const void *data, uint64_t size, uint32_t offsetX, uint32_t offsetY)
	{
	}

	void Texture2DMultisample::GenerateMipMaps() const
	{
	}
} // namespace BHive