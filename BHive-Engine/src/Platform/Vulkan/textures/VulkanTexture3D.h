#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "VulkanImage.h"

namespace BHive
{
	class BHIVE_API VulkanTexture3D : public Texture3D, public IVulkanTexture
	{
	public:
		VulkanTexture3D(const glm::uvec3& size, const FTextureCreateInfo &createInfo, const Buffer& data);

		const glm::uvec2 &GetSize() const { return {mSize.x, mSize.y}; }

		virtual void SetData(const FTextureUploadInfo &info) override {};

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return mImage.GetNativeHandle(); }

		const vk::DescriptorImageInfo GetDescriptor() const override { return mImage.GetDescriptor(); }

		const Vulkan::AllocatedImage &GetImage() const override { return mImage.GetImage(); };

		Vulkan::AllocatedImage &GetImage() override { return mImage.GetImage(); };

	private:
		vk::raii::Device &mDevice;

		glm::uvec3 mSize;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive