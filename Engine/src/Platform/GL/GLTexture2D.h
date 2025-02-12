#pragma once

#include "gfx/Texture.h"
#include "core/serialization/Serialization.h"

namespace BHive
{
	class GLTexture2D : public Texture2D
	{
	public:
		GLTexture2D() = default;
		GLTexture2D(uint32_t width, uint32_t height, const FTextureSpecification &specification, uint32_t samples = 1);
		GLTexture2D(const void *data, uint32_t width, uint32_t height, const FTextureSpecification &specification);
		~GLTexture2D();

		virtual uint32_t GetWidth() const { return mWidth; }
		virtual uint32_t GetHeight() const { return mHeight; }

		void Bind(uint32_t slot = 0) const override;
		void UnBind(uint32_t slot = 0) const override;
		void BindAsImage(uint32_t unit, uint32_t access, uint32_t level = 0) const;

		virtual void GenerateMipMaps() const;

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0);
		virtual const FTextureSpecification &GetSpecification() const { return mSpecification; }
		virtual uint32_t GetRendererID() const { return mTextureID; }

		virtual uint64_t GetResourceHandle() const { return mHandle; };
		virtual uint64_t GetImageHandle() const { return mImageHandle; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

	protected:
		void Initialize();
		void Release();

	private:
		uint32_t mTextureID = 0;
		uint32_t mSamplerID = 0;
		uint32_t mWidth = 0, mHeight = 0;
		FTextureSpecification mSpecification;
		uint32_t mSamples = 0;
		Buffer mBuffer;
		uint64_t mHandle = 0;
		uint64_t mImageHandle = 0;
	};

} // namespace BHive