#include "WindowContext.h"
#include "Platform/Vulkan/VulkanWindowContext.h"
#include "RenderCommand.h"

namespace BHive
{
	Scope<WindowContext> WindowContext::Create(void *windowHandle)
	{
		Scope<WindowContext> instance;

		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::Vulkan:
			instance = CreateScope<VulkanWindowContext>(windowHandle);
			break;
		default:
			break;
		}

		ASSERT(instance)
		sInstance = instance.get();
		return instance;
	}
} // namespace BHive