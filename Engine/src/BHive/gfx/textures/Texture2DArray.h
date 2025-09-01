#pragma once

#include "gfx/Texture.h"

namespace BHive
{
	class BHIVE_API Texture2DArray : public Texture
	{
	public:
		Texture2DArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification);

		~Texture2DArray();

		virtual uint32_t GetWidth() const { return mWidth; }

		virtual uint32_t GetHeight() const { return mHeight; }

		void Bind(uint32_t slot = 0) const override;

		void UnBind(uint32_t slot = 0) const override;

		virtual void SetData(const void *data, uint32_t offsetX = 0, uint32_t offsetY = 0);

		virtual const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		virtual uint32_t GetRendererID() const override { return mTextureID; }

	private:
		uint32_t mTextureID = 0;
		uint32_t mWidth, mHeight, mDepth;
		FTextureCreateInfo mCreateInfo;
		FTextureAPIInfo mInfo;
	};
} // namespace BHive