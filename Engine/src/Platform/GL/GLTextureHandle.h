#pragma once

#include "gfx/TextureHandle.h"

namespace BHive
{

	class GLTextureHandle : public TextureHandle
	{
	public:
		GLTextureHandle(const Ref<Texture>& texture);

		virtual ~GLTextureHandle();

		virtual uint64_t GetResourceHandle() const { return mResourceHandle; }

	private:
		uint64_t mResourceHandle = 0;
	};
}