#pragma once

#include "gfx/Texture.h"
#include "VulkanImage.h"

namespace BHive
{
	class BHIVE_API VulkanTexture2DArray : public Texture2DArray
	{
	public:
		VulkanTexture2DArray(const glm::uvec2& size, const FTextureCreateInfo &specification);

		void GenerateMips() override;

		glm::uvec2 GetSize() const override { return mSize; }

		void SetData(const FTextureUploadInfo &info) override;

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(&mImage)); }

		NativeHandle GetRenderView(uint32_t layer = 0, uint32_t mip = 0) const override;

	private:
		vk::raii::Device &mDevice;

		glm::uvec2 mSize;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive