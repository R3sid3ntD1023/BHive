#include "WindowContext.h"
#include "Platform/Vulkan/VulkanWindowContext.h"
#include "Platform/Opengl/GLWindowContext.h"
#include "RenderCommand.h"

namespace BHive
{
	Scope<WindowContext> CreateContext(Window *window)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Opengl:
			return CreateScope<GLWindowContext>(window);
		case RendererAPI::Vulkan:
			return CreateScope<VulkanWindowContext>(window);
			break;
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}

	Scope<WindowContext> WindowContext::Create(Window *window)
	{
		Scope<WindowContext> instance = CreateContext(window);

		ASSERT(instance)
		sInstance = instance.get();
		return instance;
	}
} // namespace BHive