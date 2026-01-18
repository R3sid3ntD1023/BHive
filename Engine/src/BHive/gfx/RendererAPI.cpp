#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "RendererAPI.h"

namespace BHive
{
	static const RendererAPI::EAPI sAPI = RendererAPI::Vulkan;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (sAPI)
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateScope<VulkanRendererAPI>();
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

} // namespace BHive
