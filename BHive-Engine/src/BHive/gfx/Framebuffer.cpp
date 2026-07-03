#include "Framebuffer.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"
#include "RenderCommand.h"
#include "rendergraph/Pass.h"

namespace BHive
{

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &specification)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanFramebuffer>(specification);
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}

	
} // namespace BHive