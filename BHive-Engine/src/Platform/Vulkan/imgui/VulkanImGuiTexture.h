#pragma once

#include "gfx/imgui/IImGuiProvider.h"
#include "../VulkanCore.h"
#include "gfx/ResourceID.h"

namespace BHive
{
	class VulkanImGuiTexture : public IImGuiTexture
	{
	public:
		uint64_t GetTextureID(const Texture &tex) override;

		void InvalidateTexture(const Texture &tex) override;

		void OnTextureDestroyed(ResourceID id);

	private:
		std::unordered_map<uint32_t, VkDescriptorSet> mTextureSets;
	};
}