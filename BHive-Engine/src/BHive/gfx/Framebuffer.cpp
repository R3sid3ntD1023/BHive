#include "Framebuffer.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"
#include "RenderCommand.h"

namespace BHive
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &specification)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanFramebuffer>(specification);
		}

		ASSERT(false)
		return nullptr;
	}

	
} // namespace BHive