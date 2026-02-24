#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "VulkanImage.h"

namespace BHive
{
	class BHIVE_API VulkanTexture2DArray : public Texture2DArray, public IVulkanTexture
	{
	public:
		VulkanTexture2DArray(const glm::uvec2& size, const FTextureCreateInfo &specification);

		const glm::uvec2 &GetSize() const override { return {mSize.x, mSize.y}; }

		void SetData(const FTextureUploadInfo &info) override;

		void SetLayerData(const void *data, const glm::ivec3 &offset, uint32_t layer);

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return mImage.GetNativeHandle(); }

		const vk::DescriptorImageInfo GetDescriptor() const override { return mImage.GetDescriptor(); }

		const Vulkan::AllocatedImage &GetImage() const override { return mImage.GetImage(); };

		Vulkan::AllocatedImage &GetImage() override { return mImage.GetImage(); };

	private:
		vk::raii::Device &mDevice;

		glm::uvec2 mSize;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive