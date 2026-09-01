#include "PipelineFactory.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	PipelinePtr PipelineFactory::Create()
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateResource<VulkanPipeline>();
		}

		ASSERT(false)
		return {};
	}

	PipelinePtr PipelineFactory::Create(const Pipeline::PipelineState &state)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
		{
			auto pipeline = CreateResource<VulkanPipeline>();
			pipeline.As<Pipeline>()->Init(&state);
			return pipeline;
		}
		}

		ASSERT(false)
		return {};
	}
} // namespace BHive