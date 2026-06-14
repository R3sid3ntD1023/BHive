#include "WindowContext.h"
#include "Platform/Vulkan/VulkanWindowContext.h"
#include "RenderCommand.h"

namespace BHive
{
	Scope<WindowContext> WindowContext::Create(Window* window)
	{
		Scope<WindowContext> instance;

		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			instance = CreateScope<VulkanWindowContext>(window);
			break;
		default:
			break;
		}

		ASSERT(instance)
		sInstance = instance.get();
		return instance;
	}
} // namespace BHive