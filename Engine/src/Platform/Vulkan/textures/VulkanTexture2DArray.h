#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanUtils.h"

namespace BHive
{
	class BHIVE_API VulkanTexture2DArray : public Texture2DArray
	{
	public:
		VulkanTexture2DArray(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification);

		virtual uint32_t GetWidth() const { return mWidth; }

		virtual uint32_t GetHeight() const { return mHeight; }

		void Bind(uint32_t slot = 0) const override;

		void UnBind(uint32_t slot = 0) const override;

		virtual void SetData(const void *data, uint32_t offsetX = 0, uint32_t offsetY = 0);

		virtual const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		virtual uintptr_t GetNativeHandle() const override { return reinterpret_cast<uintptr_t>(&mDescriptorInfo); }

	private:
		vk::raii::Device &mDevice;

		uint32_t mWidth, mHeight, mDepth;

		FTextureCreateInfo mCreateInfo;

		FTextureAPIInfo mInfo;

		AllocatedVulkanTexture mTextureHandle;

		vk::DescriptorImageInfo mDescriptorInfo;
	};
} // namespace BHive