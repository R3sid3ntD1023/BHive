#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanImage.h"

namespace BHive
{
	class VulkanTextureCubeArray : public TextureCubeArray
	{
	public:
		VulkanTextureCubeArray(uint32_t size, const FTextureCreateInfo &createInfo);

		void GenerateMips() override;

		glm::uvec2 GetSize() const override { return {mSize, mSize}; }

		void SetData(const FTextureUploadInfo &info) override;

		const FTextureCreateInfo &GetInfo() const { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(&mImage)); }

		NativeHandle GetRenderView(uint32_t layer = 0, uint32_t mip = 0) const override;

		void DebugPrintState() override;

	private:
		vk::raii::Device &mDevice;

		uint32_t mSize;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive