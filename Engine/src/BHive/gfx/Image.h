#pragma once

#include "core/Core.h"
#include "TextureSpecification.h"

namespace BHive
{
	class Texture;

	class Image
	{
	public:
		Image() = default;

		Image(const Ref<Texture> &texture);

		void SetTexture(const Ref<Texture> &texture);

		void Bind(uint32_t slot, EImageAccess access, uint32_t level = 0, bool layered = false, uint32_t layer = 0);

	private:
		Ref<Texture> mTexture;
		FTextureAPIInfo mTextureInfo;
	};
} // namespace BHive