#include "Image.h"
#include "Texture.h"

namespace BHive
{
	Image::Image(const Ref<Texture> &texture)
		: mTexture(texture)
	{
	}

	void Image::SetTexture(const Ref<Texture> &texture)
	{
		ASSERT(texture, "Texture not set!");

		mTexture = texture;
	}

	void Image::Bind(uint32_t slot, EImageAccess access, uint32_t level, bool layered, uint32_t layer)
	{
		ASSERT(mTexture, "No valid texture set!");

		/*auto api_access = TextureUtils::GetAPIImageAccess(access);
		glBindImageTexture(slot, *mTexture-, level, layered, layer, api_access, mTextureInfo.InternalFormat);*/
	}
} // namespace BHive