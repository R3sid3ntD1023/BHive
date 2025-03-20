#pragma once

#include "gfx/Texture.h"

namespace BHive
{
	class TextureCubeArray : public Texture
	{
	public:
		TextureCubeArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureSpecification &specification);
		~TextureCubeArray();

		virtual uint32_t GetWidth() const { return mWidth; }
		virtual uint32_t GetHeight() const { return mHeight; }

		void Bind(uint32_t slot = 0) const override;
		void UnBind(uint32_t slot = 0) const override;
		void BindAsImage(uint32_t unit, EImageAccess access, uint32_t level = 0) const;
		virtual void GenerateMipMaps() const;

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0);
		virtual const FTextureSpecification &GetSpecification() const { return mSpecification; }
		virtual uint32_t GetRendererID() const { return mTextureID; }

	private:
		uint32_t mTextureID = 0;
		uint32_t mWidth, mHeight, mDepth;
		FTextureSpecification mSpecification;
	};
} // namespace BHive