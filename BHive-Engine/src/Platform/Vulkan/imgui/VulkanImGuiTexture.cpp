#include "VulkanImGuiTexture.h"
#include "gfx/Texture.h"
#include "../VulkanImage.h"
#include <backends/imgui_impl_vulkan.h>
#include "gfx/RenderCommand.h"

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

		auto key = handle->GetResourceID();

		if (mTextureSets.contains(key))
			return (ImTextureID)mTextureSets.at(key);

		auto set = ImGui_ImplVulkan_AddTexture(smp, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		mTextureSets[key] = set;

		handle->OnDestroyed.bind(this, &VulkanImGuiTexture::OnTextureDestroyed);

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

	void VulkanImGuiTexture::OnTextureDestroyed(ResourceID id)
	{
		if (RenderCommand::IsShuttingDown())
			return;

		auto set = mTextureSets.at(id);
		ImGui_ImplVulkan_RemoveTexture(set);
		LOG_TRACE("VulkanImGuiTexture: Removed descriptor set for id {}", id);
		mTextureSets.erase(id);
	}
} // namespace BHive