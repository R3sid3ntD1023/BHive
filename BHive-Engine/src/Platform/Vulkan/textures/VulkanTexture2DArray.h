#pragma once

#include "gfx/Texture.h"
#include "VulkanImage.h"

namespace BHive
{
	class BHIVE_API VulkanTexture2DArray : public Texture2DArray
	{
	public:
		VulkanTexture2DArray(const glm::uvec2& size, const FTextureCreateInfo &specification);

		const glm::uvec2 &GetSize() const override { return {mSize.x, mSize.y}; }

		void SetData(const FTextureUploadInfo &info) override;

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return mImage.GetNativeHandle(); }

		NativeHandle GetNativeHandle() override { return mImage.GetNativeHandle(); }

		NativeHandle GetRenderView(uint32_t layer = 0, uint32_t mip = 0) const override;

	private:
		vk::raii::Device &mDevice;

		glm::uvec2 mSize;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive