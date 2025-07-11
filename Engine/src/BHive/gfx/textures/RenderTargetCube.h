#pragma once

#include "gfx/textures/TextureCube.h"
#include "gfx/cameras/CubeCamera.h"

namespace BHive
{
	class RenderTargetCube
	{
	public:
		RenderTargetCube(uint32_t size, EFormat format);
		~RenderTargetCube();

		virtual void Bind(uint32_t face);
		virtual void UnBind();
		virtual const Ref<Texture> GetTargetTexture() const { return mTargetTexture; }

	private:
		uint32_t mSize;
		uint32_t mFramebufferID = 0, mRenderBufferID;
		Ref<TextureCube> mTargetTexture;
	};
} // namespace BHive