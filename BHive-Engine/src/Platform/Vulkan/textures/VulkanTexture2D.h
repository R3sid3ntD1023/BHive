#pragma once

#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "VulkanImage.h"

namespace BHive
{

	class BHIVE_API VulkanTexture2D : public Texture2D, public IVulkanTexture
	{
	public:
		VulkanTexture2D();

		VulkanTexture2D(const glm::uvec2 &size, const FTextureCreateInfo &createInfo, const Buffer &data);

		~VulkanTexture2D();

		const glm::uvec2& GetSize() const override { return mSize; }

		void SetData(const FTextureUploadInfo &info) override;

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		void SetInfo(const FTextureCreateInfo &specs);

		Ref<Texture2D> CreateSubTexture(const FSubTexture &texture);

		void GetSubImage(const FSubTexture &texture, size_t size, uint8_t *data) const;

		const Buffer &GetBuffer() const { return mBuffer; }

		NativeHandle GetNativeHandle() const override { return mImage.GetNativeHandle(); }

		const vk::DescriptorImageInfo GetDescriptor() const override { return mImage.GetDescriptor(); }

		const Vulkan::AllocatedImage &GetImage() const override { return mImage.GetImage(); };

		Vulkan::AllocatedImage &GetImage() override { return mImage.GetImage(); };

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