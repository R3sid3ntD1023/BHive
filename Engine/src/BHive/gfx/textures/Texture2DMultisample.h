#pragma once

#include "gfx/Texture.h"

namespace BHive
{
	class Texture2DMultisample : public Texture
	{
	public:
		Texture2DMultisample(uint32_t width, uint32_t height, uint32_t samples, const FTextureSpecification &specs);

		~Texture2DMultisample();

		virtual void Bind(uint32_t slot = 0) const;

		virtual void UnBind(uint32_t slot = 0) const;

		virtual void BindAsImage(uint32_t unit, EImageAccess access, uint32_t level = 0) const;

		virtual uint32_t GetWidth() const { return mWidth; }

		virtual uint32_t GetHeight() const { return mHeight; }

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0);

		virtual uint32_t GetRendererID() const { return mTextureID; }

		virtual void GenerateMipMaps() const;

		virtual const FTextureSpecification &GetSpecification() const { return mSpecification; }

	private:
		uint32_t mWidth = 0, mHeight = 0, mSamples = 1;
		FTextureSpecification mSpecification;
		uint32_t mTextureID = 0;
	};
}