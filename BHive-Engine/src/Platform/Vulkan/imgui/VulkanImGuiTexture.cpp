#include "VulkanImGuiTexture.h"
#include "gfx/Texture.h"
#include "../VulkanImage.h"
#include <backends/imgui_impl_vulkan.h>

namespace BHive
{
	uint64_t VulkanImGuiTexture::GetTextureID(const Texture &tex)
	{
		auto handle = tex.GetNativeHandle().As<VulkanImage>();
		if (!handle)
			return 0;

		auto &native = handle->Native();
		VkSampler smp = native.GetSampler();
		VkImageView view = native.GetView(0, 0, 0);

		if (!smp || !view)
			return 0;

		auto key = tex.GetResourceID();

		if (mTextureSets.contains(key))
			return (ImTextureID)mTextureSets.at(key);

		auto set = ImGui_ImplVulkan_AddTexture(smp, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		mTextureSets[key] = set;
		return (ImTextureID)set;
	}

	void VulkanImGuiTexture::InvalidateTexture(const Texture &tex)
	{
		auto key = tex.GetResourceID();
		if (mTextureSets.contains(key))
		{
			auto set = mTextureSets.at(key);
			ImGui_ImplVulkan_RemoveTexture(set);
			mTextureSets.erase(key);
		}
	}
} // namespace BHive