#include "BackendMaterial.h"
#include "Platform/Vulkan/VulkanBackendMaterial.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	Scope<IMaterialBackendInterface> IMaterialBackendInterface::Create()
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateScope<VulkanBackendMaterial>();
		}

		ASSERT(false);
		return nullptr;
	}

}