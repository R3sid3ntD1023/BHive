#include "Texture.h"
#include "Platform/GL/GLTexture2D.h"
#include "Platform/GL/GLTextureCube.h"
#include "Platform/GL/GLTexture3D.h"

namespace BHive
{

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, const FTextureSpecification &specification, uint32_t samples)
	{
		return CreateRef<GLTexture2D>(width, height, specification, samples);
	}

	Ref<Texture2D> Texture2D::Create(const void *data, uint32_t width, uint32_t height, const FTextureSpecification &specification)
	{
		return CreateRef<GLTexture2D>(data, width, height, specification);
	}

	Ref<TextureCube> TextureCube::Create(uint32_t size, const FTextureSpecification &spec)
	{
		return CreateRef<GLTextureCube>(size, spec);
	}

	Ref<Texture> Texture3D::Create(uint32_t width, uint32_t height, uint32_t depth, const FTextureSpecification &specification)
	{
		return CreateRef<GLTexture3D>(width, height, depth, specification);
	}

	REFLECT(Texture)
	{
		BEGIN_REFLECT(Texture);
	}

	REFLECT(Texture2D)
	{
		BEGIN_REFLECT(Texture2D);
	}
}