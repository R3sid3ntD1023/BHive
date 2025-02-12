#include "GLBindlessTexture.h"
#include "gfx/Texture.h"
#include <glad/glad.h>
#include "gfx/TextureUtils.h"

namespace BHive
{
	GLBindlessTexture::GLBindlessTexture(const Ref<Texture> &texture)
	{
		mResourceHandle = glGetTextureHandleARB(texture->GetRendererID());
		if (mResourceHandle)
			glMakeTextureHandleResidentARB(mResourceHandle);
	}

	GLBindlessTexture::~GLBindlessTexture()
	{
		if (mResourceHandle)
			glMakeImageHandleNonResidentARB(mResourceHandle);
	}

	GLBindlessImage::GLBindlessImage(const Ref<Texture> &texture, uint32_t level, uint32_t access)
	{
		mResourceHandle = glGetImageHandleARB(texture->GetRendererID(), level, GL_FALSE, 0, GetGLInternalFormat(texture->GetSpecification().mFormat));
		if (mResourceHandle)
			glMakeImageHandleResidentARB(mResourceHandle, access);
	}

	GLBindlessImage::~GLBindlessImage()
	{
		if (mResourceHandle)
			glMakeImageHandleNonResidentARB(mResourceHandle);
	}

} // namespace BHive
