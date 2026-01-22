#pragma once

#include "gfx/Texture.h"

namespace BHive
{
	class VulkanTexture2DMultisample : public Texture2DMultisample
	{
	public:
		VulkanTexture2DMultisample(uint32_t width, uint32_t height, uint32_t samples, const FTextureCreateInfo &create_info);

		virtual void Bind(uint32_t slot = 0) const {};

		virtual void UnBind(uint32_t slot = 0) const {};

		virtual uint32_t GetWidth() const { return mWidth; }

		virtual uint32_t GetHeight() const { return mHeight; }

		virtual void SetData(const void *data, uint32_t offsetX = 0, uint32_t offsetY = 0) {};

		virtual const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		virtual uintptr_t GetNativeHandle() const { return 0; }

	private:
		uint32_t mWidth = 0, mHeight = 0, mSamples = 1;
		FTextureCreateInfo mCreateInfo;
		FTextureAPIInfo mInfo;
	};
} // namespace BHive