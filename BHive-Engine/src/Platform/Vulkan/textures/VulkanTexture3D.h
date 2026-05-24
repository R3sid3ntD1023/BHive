#pragma once

#include "gfx/Texture.h"
#include "VulkanImage.h"

namespace BHive
{
	class BHIVE_API VulkanTexture3D : public Texture3D
	{
	public:
		VulkanTexture3D(const glm::uvec3& size, const FTextureCreateInfo &createInfo, const Buffer& data);

		void GenerateMips() override;

		const glm::uvec2 &GetSize() const { return {mSize.x, mSize.y}; }

		virtual void SetData(const FTextureUploadInfo &info) override {};

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromRaw(reinterpret_cast<uint64_t>(&mImage)); }

		NativeHandle GetRenderView(uint32_t layer = 0, uint32_t mip = 0) const override;


	private:
		vk::raii::Device &mDevice;

		glm::uvec3 mSize;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};
} // namespace BHive