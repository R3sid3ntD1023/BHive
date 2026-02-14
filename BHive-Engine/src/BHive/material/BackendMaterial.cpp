#include "BackendMaterial.h"
#include "Platform/Vulkan/material/VulkanBackendMaterial.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	Ref<IMaterialBackendInterface> IMaterialBackendInterface::Create()
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanBackendMaterial>();
		}

		ASSERT(false);
		return nullptr;
	}

}