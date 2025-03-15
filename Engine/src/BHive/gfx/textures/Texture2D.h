#pragma once

#include "gfx/Texture.h"

namespace BHive
{
	class Texture2D : public Texture
	{
	public:
		Texture2D() = default;
		Texture2D(uint32_t width, uint32_t height, const FTextureSpecification &specification);
		Texture2D(const void *data, uint32_t width, uint32_t height, const FTextureSpecification &specification);

		virtual ~Texture2D();

		virtual void Bind(uint32_t slot = 0) const;
		virtual void UnBind(uint32_t slot = 0) const;

		virtual void BindAsImage(uint32_t unit, uint32_t access, uint32_t level = 0) const;

		virtual uint32_t GetWidth() const { return mWidth; }
		virtual uint32_t GetHeight() const { return mHeight; }

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0);
		virtual uint64_t GetResourceHandle() const { return mResourceHandle; }
		virtual uint32_t GetRendererID() const { return mTextureID; }
		virtual uint64_t GetImageHandle() const { return mImageHandle; }

		virtual void GenerateMipMaps() const;
		virtual const FTextureSpecification &GetSpecification() const { return mSpecification; }

		Ref<Texture2D> CreateSubTexture(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		void Save(cereal::BinaryOutputArchive &ar) const;
		void Load(cereal::BinaryInputArchive &ar);

		REFLECTABLEV(Texture)

	private:
		void Initialize();
		void Release();

	private:
		uint32_t mWidth = 0, mHeight = 0;
		FTextureSpecification mSpecification;
		uint64_t mResourceHandle = 0, mImageHandle = 0;
		uint32_t mTextureID = 0;
		Buffer mBuffer;
	};

	REFLECT_EXTERN(Texture2D)
} // namespace BHive