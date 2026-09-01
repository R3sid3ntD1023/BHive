#include "Pipeline.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "RenderCommand.h"
#include "ShaderManager.h"

namespace BHive
{
	Pipeline::GraphicsPipelineState Pipeline::GetDefaultGraphicsPipelineState()
	{
		GraphicsPipelineState state{};

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
		state.Blend.DstAlpha = EBlendFactor::OneMinusSrcAlpha;
		state.Blend.AlphaOp = EBlendOp::Add;

		state.Depth.DepthTest = true;
		state.Depth.DepthWrite = true;
		state.Depth.DepthCompare = ECompareOp::LessOrEqual;

		return state;
	}
} // namespace BHive