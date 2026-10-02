#include "VulkanImGuiTexture.h"
#include "../VulkanImage.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "gfx/RenderCommand.h"
#include "gfx/Texture.h"
#include <backends/imgui_impl_vulkan.h>

namespace BHive
{
	uint64_t VulkanImGuiTexture::GetTextureID(const Texture &tex)
	{
		auto handle = tex.GetTextureID();
		if (handle == -1)
			return 0;

		auto image = VulkanRendererAPI::GetInstance()->GetTexture(handle);
		VkSampler smp = image->GetSampler();
		VkImageView view = image->GetView(0, 0);

		if (!smp || !view)
			return 0;

		if (mTextureSets.contains(handle))
			return (ImTextureID)mTextureSets.at(handle);

		auto set = ImGui_ImplVulkan_AddTexture(smp, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		mTextureSets[handle] = set;

		image->OnDestroyed.bind(this, &VulkanImGuiTexture::OnTextureDestroyed);

		return (ImTextureID)set;
	}

	void VulkanImGuiTexture::InvalidateTexture(const Texture &tex)
	{
		auto handle = tex.GetTextureID();
		if (mTextureSets.contains(handle))
		{
			auto set = mTextureSets.at(handle);
			ImGui_ImplVulkan_RemoveTexture(set);
			mTextureSets.erase(handle);
		}
	}

	void VulkanImGuiTexture::OnTextureDestroyed(ResourceID id)
	{
		if (RenderCommand::IsShuttingDown())
			return;

		if (!mTextureSets.contains(id))
			return;

		auto set = mTextureSets.at(id);
		ImGui_ImplVulkan_RemoveTexture(set);
		LOG_TRACE("VulkanImGuiTexture: Removed descriptor set for id {}", id);
		mTextureSets.erase(id);
	}
} // namespace BHive