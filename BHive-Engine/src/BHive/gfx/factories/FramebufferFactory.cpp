#include "FramebufferFactory.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	FramebufferPtr BHive::FramebufferFactory::Create(const FramebufferSpecification &specification)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateResource<VulkanFramebuffer>(specification);
		default:
			break;
		}

		ASSERT(false)
		return {};
	}
} // namespace BHive