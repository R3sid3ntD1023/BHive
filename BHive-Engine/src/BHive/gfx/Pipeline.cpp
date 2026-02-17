#include "Pipeline.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "RenderCommand.h"

namespace BHive
{
	Pipeline::PipelineState Pipeline::GetDefaultPipelineState()
	{
		PipelineState state{};

		state.Raster.CullEnabled = true;
		state.Raster.CullMode = ECullMode::Back;
		state.Raster.FrontFace = EFrontFace::CWW;
		state.Raster.FillMode = EPolygonMode::Fill;

		state.DrawMode = ETopologyMode::Triangles;
		state.Blend.Enabled = true;
		state.Blend.SrcColor = EBlendFactor::SrcAlpha;
		state.Blend.DstColor = EBlendFactor::OneMinusSrcAlpha;
		state.Blend.ColorOp = EBlendOp::Add;
		state.Blend.SrcAlpha = EBlendFactor::One;
		state.Blend.DstAlpha = EBlendFactor::Zero;
		state.Blend.AlphaOp = EBlendOp::Add;

		state.Depth.DepthTest = true;
		state.Depth.DepthWrite = true;
		state.Depth.DepthCompare = ECompareOp::LessOrEqual;
		state.DepthAttachmentFormat = EFormat::DEPTH24_STENCIL8;

		state.ColorAttachmentFormats = {EFormat::RGBA8};

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