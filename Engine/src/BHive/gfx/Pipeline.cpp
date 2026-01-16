#include "Pipeline.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "RenderCommand.h"

namespace BHive
{
	Ref<Pipeline> BHive::Pipeline::Create(const Configuration &configuration)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanPipeline>((const FVulkanPipelineConfigInfo &)configuration);
		}
	}
} // namespace BHive