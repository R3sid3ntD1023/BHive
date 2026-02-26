#pragma once

#include "gfx/Texture.h"
#include "VulkanImage.h"
#include "Platform/Vulkan/IVulkanTexture.h"

namespace BHive
{

	class BHIVE_API VulkanTextureCube : public TextureCube, public IVulkanTexture
	{
	public:
		VulkanTextureCube() = default;

		VulkanTextureCube(uint32_t size, const FTextureCreateInfo &createInfo);

		const glm::uvec2& GetSize() const  override{ return {mSize, mSize}; }

		virtual void SetData(const FTextureUploadInfo &info) {}

		virtual const FTextureCreateInfo &GetInfo() const { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return mImage.GetNativeHandle(); }

		const vk::DescriptorImageInfo GetDescriptor() const override { return mImage.GetDescriptor(); }
	
		virtual const AllocatedImage &GetImage() const override { return mImage.GetImage(); };

		virtual AllocatedImage &GetImage() override { return mImage.GetImage(); };

	private:
		vk::raii::Device &mDevice;
		uint32_t mSize{0};
		FTextureCreateInfo mCreateInfo;
		VulkanImage mImage;
	};
} // namespace BHive