#include "IImGuiProvider.h"
#include "Platform/Vulkan/imgui/VulkanImGuiTexture.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	void IImGuiTextureProvider::Init()
	{
		auto api = RenderCommand::GetAPI();
		switch (api)
		{
		case BHive::RendererAPI::Vulkan:
		{
			sBackend = CreateScope<VulkanImGuiTexture>();
		}
		break;
		default:
			break;
		}
	}

	void IImGuiTextureProvider::Shutdown()
	{
		sBackend.reset();
	}

	uint64_t IImGuiTextureProvider::GetID(const Texture &tex)
	{
		return sBackend->GetTextureID(tex);
	}

	void IImGuiTextureProvider::Invalidate(const Texture &tex)
	{
		sBackend->InvalidateTexture(tex);
	}
} // namespace BHive