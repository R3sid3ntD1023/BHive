#include "BindlessTexture.h"
#include "Platform/GL/GLBindlessTexture.h"

namespace BHive
{
	Ref<BindlessTexture> BindlessTexture::Create(const Ref<Texture> &texture)
	{
		return CreateRef<GLBindlessTexture>(texture);
	}

	Ref<BindlessImage> BindlessImage::Create(const Ref<Texture> &texture, uint32_t level, uint32_t access)
	{
		return CreateRef<GLBindlessImage>(texture, level, access);
	}
} // namespace BHive
