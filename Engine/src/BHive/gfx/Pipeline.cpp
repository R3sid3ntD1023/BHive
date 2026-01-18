#include "Pipeline.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "RenderCommand.h"

namespace BHive
{
	Ref<Pipeline> BHive::Pipeline::Create()
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanPipeline>();
		}

		ASSERT(false)
		return nullptr;
	}
} // namespace BHive