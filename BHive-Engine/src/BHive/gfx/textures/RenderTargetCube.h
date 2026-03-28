#pragma once

#include "gfx/Enumerations.h"

namespace BHive
{
	class Framebuffer;
	class TextureCube;
	class Texture;

	class RenderTargetCube
	{
	public:
		RenderTargetCube(uint32_t size, EFormat format);

		virtual void Bind(uint32_t face);

		virtual void UnBind();

		virtual const Ref<Texture> GetTargetTexture() const;

	private:
		uint32_t mSize;
		Ref<Framebuffer> mFrameBuffer;
		Ref<TextureCube> mTargetTexture;
	};
} // namespace BHive