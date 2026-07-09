#pragma once

#include "gfx/imgui/IImGuiProvider.h"
#include "../VulkanCore.h"

namespace BHive
{
	class VulkanImGuiTexture : public IImGuiTexture
	{
	public:
		~VulkanImGuiTexture();

		uint64_t GetTextureID(const Texture &tex) override;

		void InvalidateTexture(const Texture &tex) override;

	private:
		std::unordered_map<uint32_t, VkDescriptorSet> mTextureSets;
	};
}