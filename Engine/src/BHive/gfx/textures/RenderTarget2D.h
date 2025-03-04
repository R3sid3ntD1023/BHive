#pragma once

#include "gfx/Texture.h"

namespace BHive
{
	class RenderTarget2D
	{
	public:
		RenderTarget2D(const Ref<Texture> &texture, uint32_t attachment = 0, uint32_t level = 0);
		~RenderTarget2D();

		virtual void Bind();
		virtual void UnBind();
		virtual const Ref<Texture> GetTargetTexture() const { return mTargetTexture; }

	private:
		uint32_t mWidth = 0, mHeight = 0;
		uint32_t mFramebufferID = 0, mRenderBufferID = 0;
		Ref<Texture> mTargetTexture;
	};
} // namespace BHive