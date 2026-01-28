#include "GraphicsContext.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"
#include "RenderCommand.h"

namespace BHive
{
	Scope<GraphicsContext> GraphicsContext::Create(void *windowHandle)
	{
		Scope<GraphicsContext> instance;

		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::Vulkan:
			instance = CreateScope<VulkanGraphicsContext>(windowHandle);
			break;
		default:
			break;
		}

		ASSERT(instance)
		sInstance = instance.get();
		return instance;
	}
} // namespace BHive