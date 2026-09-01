#include "CircleRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{
#define CIRCLE_PIPELINE_NAME "CirclePipeline"

	void CircleRenderBatch::Initialize()
	{
		mBuffer = CreateScope<VertexBatchBuffer<CircleVertex>>(sMaxVertexCount, sMaxIndexCount, true);

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;

		mCircleMaterial = MaterialFactory::Create("Circle.glsl");
		mPipeline = PipelineFactory::Create(state);
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
		pass.Emplace<CmdBindPipeline>()(mPipeline);
		pass.Emplace<CmdBindMaterial>()(mCircleMaterial.As<Material>());
		pass.Emplace<CmdDraw>()(ETopologyMode::Triangles, mBuffer->GetVAO(), mBuffer->GetIndexCount());

		mIsActive = false;
	}

	bool CircleRenderBatch::IsFull(uint32_t vNeeded, uint32_t iNeeded)
	{
		return (mBuffer->GetVertexCount() + vNeeded > sMaxVertexCount) || (mBuffer->GetIndexCount() + iNeeded > sMaxIndexCount);
	}
} // namespace BHive