#include "BackendMaterial.h"
#include "Platform/Vulkan/VulkanBackendMaterial.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	Ref<IMaterialBackendInterface> IMaterialBackendInterface::Create(const std::string &shaderProgramName)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanBackendMaterial>(shaderProgramName);
		}

		ASSERT(false);
		return nullptr;
	}

} // namespace BHive