#pragma once

#include "Texture.h"

namespace BHive
{
	class TextureRenderTarget
	{
	public:
		TextureRenderTarget(const Ref<Texture> &texture, uint32_t attachment = 0, uint32_t level = 0);
		~TextureRenderTarget();

		virtual void Bind();
		virtual void UnBind();
		virtual const Ref<Texture> GetTargetTexture() const { return mTargetTexture; }

	private:
		uint32_t mWidth = 0, mHeight = 0;
		uint32_t mFramebufferID = 0, mRenderBufferID = 0;
		Ref<Texture> mTargetTexture;
	};
} // namespace BHive