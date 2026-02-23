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

		VulkanTexture2D(uint32_t w, uint32_t h, const FTextureCreateInfo &info = {}, const void *buffer = nullptr, size_t size = 0);

		~VulkanTexture2D();

		virtual void Bind(uint32_t slot = 0) const;

		virtual void UnBind(uint32_t slot = 0) const;

		virtual uint32_t GetWidth() const { return mWidth; }

		virtual uint32_t GetHeight() const { return mHeight; }

		virtual void SetData(const void *data, const glm::uvec3 &offset = {0, 0, 0}) override;

		virtual const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		void SetInfo(const FTextureCreateInfo &specs);

		Ref<Texture2D> CreateSubTexture(const FSubTexture &texture);

		void GetSubImage(const FSubTexture &texture, size_t size, uint8_t *data) const;

		const Buffer &GetBuffer() const { return mBuffer; }

		NativeHandle GetNativeHandle() const override { return mImage.GetNativeHandle(); }

		const vk::DescriptorImageInfo GetDescriptor() const override { return mImage.GetDescriptor(); }

		virtual const Vulkan::AllocatedImage &GetImage() const override { return mImage.GetImage(); };

		virtual Vulkan::AllocatedImage &GetImage() override { return mImage.GetImage(); };

		/*Begin Asset*/
		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Texture2D)

		/*End Asset*/

	private:
		void Initialize();

	private:
		vk::raii::Device &mDevice;

		VulkanImage mImage;

		FTextureCreateInfo mCreateInfo;

		Buffer mBuffer;

		uint32_t mWidth = 0, mHeight = 0;
	};

} // namespace BHive