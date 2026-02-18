#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "VulkanImage.h"

namespace BHive
{
	class BHIVE_API VulkanTexture3D : public Texture3D, public IVulkanTexture
	{
	public:
		VulkanTexture3D(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &create_info, const void *data = nullptr);

		virtual uint32_t GetWidth() const { return mWidth; }

		virtual uint32_t GetHeight() const { return mHeight; }

		void Bind(uint32_t slot = 0) const override {};

		void UnBind(uint32_t slot = 0) const override {};

		virtual void SetData(const void *data, uint32_t offsetX = 0, uint32_t offsetY = 0) {};

		virtual const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		virtual NativeHandle GetNativeHandle() const;

		virtual const vk::ImageView &GetImageView() const override { return mImage.GetAllocatedTexture().View; }

	private:
		vk::raii::Device &mDevice;

		uint32_t mWidth, mHeight, mDepth;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive