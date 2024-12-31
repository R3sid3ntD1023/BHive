#include "TextureHandle.h"
#include "Platform/GL/GLTextureHandle.h"

namespace BHive
{
	Ref<TextureHandle> TextureHandle::Create(const Ref<Texture>& texture)
	{
		return CreateRef<GLTextureHandle>(texture);
	}
}
