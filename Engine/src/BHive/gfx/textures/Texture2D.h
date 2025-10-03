#pragma once

#include "gfx/Texture.h"
#include "gfx/VulkanCore.h"

namespace BHive
{

	class BHIVE_API Texture2D : public Texture
	{
	public:
		Texture2D() = default;
		Texture2D(uint32_t w, uint32_t h, const FTextureCreateInfo &info = {}, const void *buffer = nullptr, size_t size = 0);

		virtual ~Texture2D();

		virtual void Bind(uint32_t slot = 0) const;

		virtual void UnBind(uint32_t slot = 0) const;

		virtual uint32_t GetWidth() const { return mWidth; }

		virtual uint32_t GetHeight() const { return mHeight; }

		virtual void SetData(const void *data, uint32_t offsetX = 0, uint32_t offsetY = 0);

		virtual uint32_t GetRendererID() const { return mTextureID; }

		virtual const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		void SetInfo(const FTextureCreateInfo &specs);

		Ref<Texture2D> CreateSubTexture(const FSubTexture &texture);

		void GetSubImage(const FSubTexture &texture, size_t size, uint8_t *data) const;

		const Buffer &GetBuffer() const { return mBuffer; }

		vk::raii::ImageView& GetView()  { return mTextureImageView; }

		vk::raii::Sampler& GetSampler() { return mTextureSampler; }

		/*Begin Asset*/
		void Save(cereal::BinaryOutputArchive &ar) const override;
		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Texture)

		/*End Asset*/

	private:
		void Initialize();
		void Release();

	private:
		uint32_t mWidth = 0;
		uint32_t mHeight = 0;
		FTextureCreateInfo mCreateInfo;
		FTextureAPIInfo mInfo;
		uint32_t mTextureID = 0;
		Buffer mBuffer;

		vk::raii::Image mTextureImage = nullptr;
		vk::raii::DeviceMemory mTextureImageMemory = nullptr;
		vk::raii::ImageView mTextureImageView = nullptr;
		vk::raii::Sampler mTextureSampler = nullptr;
	};

	REFLECT_EXTERN(Texture2D)

} // namespace BHive