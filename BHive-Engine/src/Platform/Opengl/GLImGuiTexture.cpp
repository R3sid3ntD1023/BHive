#include "GLImGuiTexture.h"
#include "gfx/Texture.h"

namespace BHive
{
	uint64_t GLImGuiTexture::GetTextureID(const Texture &tex)
	{
		return static_cast<uint64_t>(tex.GetTextureID());
	}

	void GLImGuiTexture::InvalidateTexture(const Texture &)
	{
	}

} // namespace BHive