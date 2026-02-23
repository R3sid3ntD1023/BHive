#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "VulkanImage.h"

namespace BHive
{
	class VulkanTextureCubeArray : public TextureCubeArray, public IVulkanTexture
	{
	public:
		VulkanTextureCubeArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification);

		virtual uint32_t GetWidth() const { return mWidth; }

		virtual uint32_t GetHeight() const { return mHeight; }

		void Bind(uint32_t slot = 0) const override;

		void UnBind(uint32_t slot = 0) const override;

		virtual void SetData(const void *data, const glm::uvec3 &offset = {0, 0, 0}) override;

		virtual const FTextureCreateInfo &GetInfo() const { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return mImage.GetNativeHandle(); }

		const vk::DescriptorImageInfo GetDescriptor() const override { return mImage.GetDescriptor(); }

		virtual const Vulkan::AllocatedImage &GetImage() const override { return mImage.GetImage(); };

		virtual Vulkan::AllocatedImage &GetImage() override { return mImage.GetImage(); };

	private:
		vk::raii::Device &mDevice;

		uint32_t mWidth, mHeight, mDepth;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive