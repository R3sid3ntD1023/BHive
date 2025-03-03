#pragma once

#include "gfx/Texture.h"

namespace BHive
{
	class Framebuffer;

	class GLTextureCube : public TextureCube
	{
	public:
		GLTextureCube() = default;
		GLTextureCube(uint32_t size, const FTextureSpecification &spec);
		~GLTextureCube();

		virtual void Bind(uint32_t slot = 0) const;
		void BindAsImage(uint32_t unit, uint32_t access, uint32_t level = 0) const;
		virtual void UnBind(uint32_t slot = 0) const;
		virtual void GenerateMipMaps() const;

		virtual uint32_t GetWidth() const { return mSize; }
		virtual uint32_t GetHeight() const { return mSize; }

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0) {}
		virtual const FTextureSpecification &GetSpecification() const { return mSpecification; }
		virtual uint32_t GetRendererID() const { return mTextureID; }
		virtual uint64_t GetResourceHandle() const { return mResourceHandle; };
		// virtual uint64_t GetImageHandle() const { return mImageHandle; }

		void AttachToFramebuffer(const Ref<Framebuffer> &framebuffer, uint32_t attachment, uint32_t level = 0);

	private:
		uint32_t mSize;
		FTextureSpecification mSpecification;
		uint32_t mTextureID{0};
		uint32_t mTextureTarget{0};
		uint64_t mResourceHandle = 0;
		// uint64_t mImageHandle = 0;
	};
} // namespace BHive