#pragma once

#include "gfx/Texture.h"
#include "VulkanImage.h"

namespace BHive
{
	class VulkanTextureCubeArray : public TextureCubeArray
	{
	public:
		VulkanTextureCubeArray(uint32_t size, const FTextureCreateInfo &createInfo);

		const glm::uvec2 &GetSize() const override { return {mSize, mSize}; }

		void SetData(const FTextureUploadInfo &info) override;

		const FTextureCreateInfo &GetInfo() const { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return mImage.GetNativeHandle(); }

		NativeHandle GetNativeHandle() override { return mImage.GetNativeHandle(); }

		NativeHandle GetRenderView(uint32_t layer = 0, uint32_t mip = 0) const override;

	private:
		vk::raii::Device &mDevice;

		uint32_t mSize;

		FTextureCreateInfo mCreateInfo;

		ImageCube mImage;
	};
} // namespace BHive