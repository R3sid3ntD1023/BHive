#include "GLTextureHandle.h"
#include "gfx/Texture.h"
#include <glad/glad.h>

namespace BHive
{
	GLTextureHandle::GLTextureHandle(const Ref<Texture>& texture)
	{
		mResourceHandle = glGetTextureHandleARB(texture->GetRendererID());
		glMakeTextureHandleResidentARB(mResourceHandle);
	}

	GLTextureHandle::~GLTextureHandle()
	{
		//glMakeImageHandleNonResidentARB(mResourceHandle);
	}
}