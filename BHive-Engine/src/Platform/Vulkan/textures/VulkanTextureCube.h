#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanImage.h"

namespace BHive
{

	class BHIVE_API VulkanTextureCube final : public TextureCube, public IVulkanTextureInterface
	{
	public:
		VulkanTextureCube() = default;

		VulkanTextureCube(uint32_t size, const FTextureCreateInfo &createInfo);

		glm::uvec2 GetSize() const  override{ return {mSize, mSize}; }

		virtual void SetData(const FTextureUploadInfo &info) {}

		void Initilaize();

		virtual const FTextureCreateInfo &GetInfo() const { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mImage); }

		VkImageView ResolveRenderView(uint32_t face = 0, uint32_t mip = 0) const override;

		void DebugPrintState() override;

	private:
		vk::raii::Device &mDevice;
		uint32_t mSize{0};
		FTextureCreateInfo mCreateInfo;
		VulkanImage mImage;
	};
} // namespace BHive