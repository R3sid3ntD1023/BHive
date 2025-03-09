#pragma once

#include "gfx/textures/TextureCube.h"

namespace BHive
{
	class RenderTarget2D
	{
	public:
		RenderTarget2D(uint32_t size, float radius);
		~RenderTarget2D();

		virtual void Bind(uint32_t face);
		virtual void UnBind();
		virtual const Ref<Texture> GetTargetTexture() const { return mTargetTexture; }

	private:
		uint32_t mSize;
		float mRadius;
		uint32_t mFramebufferID = 0, mRenderBufferID;
		Ref<TextureCube> mTargetTexture;
	};
} // namespace BHive