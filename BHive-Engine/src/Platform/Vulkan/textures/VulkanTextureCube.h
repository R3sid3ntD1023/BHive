#pragma once

#include "gfx/Texture.h"
#include "VulkanImage.h"

namespace BHive
{

	class BHIVE_API VulkanTextureCube final: public TextureCube
	{
	public:
		VulkanTextureCube() = default;

		VulkanTextureCube(uint32_t size, const FTextureCreateInfo &createInfo);

		const glm::uvec2& GetSize() const  override{ return {mSize, mSize}; }

		virtual void SetData(const FTextureUploadInfo &info) {}

		void Initilaize();

		virtual const FTextureCreateInfo &GetInfo() const { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return mImage.GetNativeHandle(); }

		NativeHandle GetNativeHandle() override { return mImage.GetNativeHandle(); }

		NativeHandle GetRenderView(uint32_t face = 0, uint32_t mip = 0) const override;

	private:
		vk::raii::Device &mDevice;
		uint32_t mSize{0};
		FTextureCreateInfo mCreateInfo;
		ImageCube mImage;
	};
} // namespace BHive