#include "BackendMaterial.h"
#include "Platform/Vulkan/VulkanBackendMaterial.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	Ref<IMaterialBackendInterface> IMaterialBackendInterface::Create()
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanBackendMaterial>();
		}

		ASSERT(false);
		return nullptr;
	}

}