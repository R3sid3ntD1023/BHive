#include "BackendMaterial.h"
#include "Platform/Vulkan/VulkanBackendMaterial.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	Ref<IMaterialBackendInterface> IMaterialBackendInterface::Create(const Ref<ShaderProgram> &program)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanBackendMaterial>(program);
		}

		ASSERT(false);
		return nullptr;
	}

} // namespace BHive