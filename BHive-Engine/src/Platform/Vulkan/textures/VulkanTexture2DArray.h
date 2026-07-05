#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanImage.h"

namespace BHive
{
	class BHIVE_API VulkanTexture2DArray : public Texture2DArray, public IVulkanTextureInterface
	{
	public:
		VulkanTexture2DArray(const glm::uvec2& size, const FTextureCreateInfo &specification);

		glm::uvec2 GetSize() const override { return mSize; }

		void SetData(const FTextureUploadInfo &info) override;

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mImage); }

		VkImageView ResolveRenderView(uint32_t layer = 0, uint32_t mip = 0) const override;

		void DebugPrintState() override;

	private:
		vk::raii::Device &mDevice;

		glm::uvec2 mSize;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive