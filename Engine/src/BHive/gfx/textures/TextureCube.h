#pragma once

#include "gfx/Texture.h"
#include "gfx/VulkanUtils.h"

namespace BHive
{

	class BHIVE_API TextureCube : public Texture
	{
	public:
		TextureCube() = default;

		TextureCube(uint32_t size, const FTextureCreateInfo &info);

		~TextureCube();

		virtual void Bind(uint32_t slot = 0) const;

		virtual void UnBind(uint32_t slot = 0) const;

		virtual uint32_t GetWidth() const { return mSize; }

		virtual uint32_t GetHeight() const { return mSize; }

		virtual void SetData(const void *data, uint32_t offsetX = 0, uint32_t offsetY = 0) {}

		virtual const FTextureCreateInfo &GetInfo() const { return mCreateInfo; }

		virtual uint32_t GetRendererID() const { return mTextureID; }

		virtual uintptr_t GetNativeHandle() const { return reinterpret_cast<uintptr_t>(&mDescriptorInfo); }

	private:
		uint32_t mSize{0};
		FTextureCreateInfo mCreateInfo;
		FTextureAPIInfo mInfo;
		uint32_t mTextureID{0};
		uint32_t mTextureTarget{0};
		AllocatedVulkanTexture mTextureHandle;
		vk::DescriptorImageInfo mDescriptorInfo;
	};
} // namespace BHive