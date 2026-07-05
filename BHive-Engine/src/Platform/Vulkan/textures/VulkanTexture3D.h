#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanImage.h"

namespace BHive
{
	class BHIVE_API VulkanTexture3D : public Texture3D, public IVulkanTextureInterface
	{
	public:
		VulkanTexture3D(const glm::uvec3& size, const FTextureCreateInfo &createInfo, const Buffer& data);

		glm::uvec2 GetSize() const override { return {mSize.x, mSize.y}; }

		void SetData(const FTextureUploadInfo &info) override {};

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mImage); }

		VkImageView ResolveRenderView(uint32_t layer = 0, uint32_t mip = 0) const override;

		void DebugPrintState() override;

	private:
		vk::raii::Device &mDevice;

		glm::uvec3 mSize;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive