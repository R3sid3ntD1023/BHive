#include "gfx/RenderCommand.h"
#include "gfx/ShaderManager.h"
#include "LineRenderBatch.h"
#include "gfx/material/Material.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{
#define LINE_PIPELINE_NAME "LinePipeline"

	void LineRenderBatch::Initialize()
	{
		mBuffer = CreateScope<VertexBatchBuffer<FLineVertex>>(sMaxVertexCount, 0, false);

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.Raster.CullEnabled = false;
		state.DrawMode = ETopologyMode::Lines;

		PipelineRegistry::Register(LINE_PIPELINE_NAME, state);

		mLineMaterial = CreateScope<Material>("Line.glsl");
	}

	bool LineRenderBatch::NeedsFlush(uint32_t vNeeded, uint32_t iNeeded)
	{
		return IsFull(vNeeded, iNeeded);
	}

	void LineRenderBatch::StartBatch()
	{
		mBuffer->Reset();
		mIsActive = true;
	}

	void LineRenderBatch::Flush(Renderer &renderer)
	{
		if (mBuffer->GetVertexCount() == 0)
			return;

		mBuffer->Upload();

		auto &pass = renderer.GetActivePass();
		pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get(LINE_PIPELINE_NAME));
		pass.Emplace<CmdSetLineWidth>()(glm::max(0.01f, mLineWidth));
		pass.Emplace<CmdBindMaterial>()(mLineMaterial.get());
		pass.Emplace<CmdDraw>()(ETopologyMode::Lines, mBuffer->GetVAO(), mBuffer->GetVertexCount());

		mIsActive = false;
	}

	void LineRenderBatch::SetLineWidth(float width)
	{
		mLineWidth = width;
	}

	bool LineRenderBatch::IsFull(uint32_t vNeeded, uint32_t iNeeded)
	{
		return (mBuffer->GetVertexCount() + vNeeded) > sMaxVertexCount;
	}
} // namespace BHive