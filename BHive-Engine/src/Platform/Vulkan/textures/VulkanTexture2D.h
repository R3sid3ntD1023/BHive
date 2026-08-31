#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanImage.h"

namespace BHive
{

	class BHIVE_API VulkanTexture2D : public Texture2D, public IVulkanTextureInterface
	{
	public:
		VulkanTexture2D();

		VulkanTexture2D(const glm::uvec2 &size, const FTextureCreateInfo &createInfo, const Buffer &data);

		~VulkanTexture2D();

		glm::uvec2 GetSize() const override { return mSize; }

		void SetData(const FTextureUploadInfo &info) override;

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		void SetInfo(const FTextureCreateInfo &specs);

		const Buffer &GetBuffer() const { return mBuffer; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mImage); }

		VkImageView ResolveRenderView(uint32_t layer = 0, uint32_t mip = 0) const override;

		void DebugPrintState() override;

		/*Begin Asset*/
		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Texture2D)

		/*End Asset*/

	private:
		void Initialize();

	private:
		vk::raii::Device &mDevice;

		glm::uvec2 mSize{0, 0};

		Buffer mBuffer;

		FTextureCreateInfo mCreateInfo;

		VulkanImage mImage;
	};

} // namespace BHive