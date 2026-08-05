#include "CircleRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/material/Material.h"
#include "gfx/Pipeline.h"

namespace BHive
{
#define CIRCLE_PIPELINE_NAME "CirclePipeline"

	void CircleRenderBatch::Initialize()
	{
		mBuffer = CreateScope<VertexBatchBuffer<CircleVertex>>(sMaxVertexCount, sMaxIndexCount, true);

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;

		PipelineRegistry::Register(CIRCLE_PIPELINE_NAME, state);

		auto shader = ShaderManager::Get("Circle.glsl");
		mCircleMaterial = CreateScope<Material>(shader);
	}

	void CircleRenderBatch::StartBatch()
	{
		mBuffer->Reset();

		mIsActive = true;
	}

	bool CircleRenderBatch::NeedsFlush(uint32_t vNeeded, uint32_t iNeeded)
	{
		return IsFull(vNeeded, iNeeded);
	}

	void CircleRenderBatch::Flush(Renderer &renderer)
	{
		if (mBuffer->GetIndexCount() == 0)
			return;

		mBuffer->Upload();

		auto &pass = renderer.GetActivePass();
		pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get(CIRCLE_PIPELINE_NAME));
		pass.Emplace<CmdBindMaterial>()(mCircleMaterial.get());
		pass.Emplace<CmdDraw>()(ETopologyMode::Triangles, mBuffer->GetVAO(), mBuffer->GetIndexCount());

		mIsActive = false;
	}

	bool CircleRenderBatch::IsFull(uint32_t vNeeded, uint32_t iNeeded)
	{
		return (mBuffer->GetVertexCount() + vNeeded > sMaxVertexCount) || (mBuffer->GetIndexCount() + iNeeded > sMaxIndexCount);
	}
} // namespace BHive