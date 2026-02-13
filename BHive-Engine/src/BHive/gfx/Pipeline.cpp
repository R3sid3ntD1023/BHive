#include "Pipeline.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "RenderCommand.h"

namespace BHive
{
	Pipeline::PipelineState Pipeline::GetDeafultPipelineState()
	{
		PipelineState state{};

		state.Raster.CullEnabled = true;
		state.Raster.CullMode = ECullMode::Back;
		state.Raster.FrontFace = EFrontFace::CWW;
		state.Raster.FillMode = EPolygonMode::Fill;

		state.DrawMode = ETopologyMode::TriangleStrip;
		state.Blend.Enabled = false;
		state.Depth.DepthTest = false;
		state.Depth.DepthWrite = false;

		return state;
	}

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