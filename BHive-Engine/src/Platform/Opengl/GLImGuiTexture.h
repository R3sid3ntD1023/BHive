#pragma once

#include "gfx/imgui/IImGuiProvider.h"

namespace BHive
{
	class GLImGuiTexture : public IImGuiTexture
	{
	public:
		uint64_t GetTextureID(const Texture &tex) override;

		void InvalidateTexture(const Texture &tex) override;

	};
}