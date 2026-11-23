#include "PipelineFactory.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Opengl/GLPipeline.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	PipelinePtr PipelineFactory::Create()
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Opengl:
			return CreateResource<GLPipeline>();
		case RendererAPI::Vulkan:
			return CreateResource<VulkanPipeline>();
		}

		ASSERT(false)
		return {};
	}

	PipelinePtr PipelineFactory::Create(const Pipeline::PipelineState &state)
	{
		PipelinePtr pipeline;

		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Opengl:
			pipeline = CreateResource<GLPipeline>();
			break;
		case RendererAPI::Vulkan:

			pipeline = CreateResource<VulkanPipeline>();
			break;
		}

		ASSERT(pipeline)
		pipeline.As<Pipeline>()->Init(&state);
		return pipeline;
	}
} // namespace BHive