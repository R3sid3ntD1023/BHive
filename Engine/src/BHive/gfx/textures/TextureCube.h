#pragma once

#include "gfx/Texture.h"

namespace BHive
{

	class BHIVE_API TextureCube : public Texture
	{
	public:
		TextureCube() = default;
		TextureCube(uint32_t size, const FTextureSpecification &spec);
		~TextureCube();

		virtual void Bind(uint32_t slot = 0) const;
		void BindAsImage(uint32_t unit, EImageAccess access, uint32_t level = 0) const;
		virtual void UnBind(uint32_t slot = 0) const;
		virtual void GenerateMipMaps() const;

		virtual uint32_t GetWidth() const { return mSize; }
		virtual uint32_t GetHeight() const { return mSize; }

		virtual void SetData(const void *data, uint32_t offsetX = 0, uint32_t offsetY = 0) {}
		virtual const FTextureSpecification &GetSpecification() const { return mSpecification; }
		virtual uint32_t GetRendererID() const { return mTextureID; }

	private:
		uint32_t mSize;
		FTextureSpecification mSpecification;
		uint32_t mTextureID{0};
		uint32_t mTextureTarget{0};
	};
} // namespace BHive