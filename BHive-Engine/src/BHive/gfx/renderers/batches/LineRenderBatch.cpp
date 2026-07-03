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

		auto shader = ShaderManager::Get("Line.glsl");

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.ShaderProgram = shader;
		state.Raster.CullEnabled = false;
		state.DrawMode = ETopologyMode::Lines;

		PipelineRegistry::Register(LINE_PIPELINE_NAME, state);

		mLineMaterial = CreateScope<Material>();
		mLineMaterial->SetPipeline(PipelineRegistry::Get(LINE_PIPELINE_NAME));
		mLineMaterial->Submit();
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

	void LineRenderBatch::Flush(Renderer& renderer)
	{
		if (mBuffer->GetVertexCount() == 0)
			return;

		mBuffer->Upload();

		auto& pass = renderer.GetActivePass();
		pass.Emplace<CmdSetLineWidth>()(2.0f);
		pass.Emplace<CmdBindMaterial>()(mLineMaterial.get());
		pass.Emplace<CmdDraw>()(ETopologyMode::Lines, mBuffer->GetVAO(), mBuffer->GetVertexCount());

		mIsActive = false;
	}

	bool LineRenderBatch::IsFull(uint32_t vNeeded, uint32_t iNeeded)
	{
		return (mBuffer->GetVertexCount() + vNeeded) > sMaxVertexCount;
	}
} // namespace BHive