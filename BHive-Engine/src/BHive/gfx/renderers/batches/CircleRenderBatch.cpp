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

		auto shaderProgram = ShaderManager::Get("Circle.glsl");

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.ShaderProgram = shaderProgram;
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;

		PipelineRegistry::Register(CIRCLE_PIPELINE_NAME, state);

		mCircleMaterial = CreateScope<Material>();
		mCircleMaterial->SetPipeline(PipelineRegistry::Get(CIRCLE_PIPELINE_NAME));
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

	void CircleRenderBatch::Flush(Renderer& renderer)
	{
		if (mBuffer->GetIndexCount() == 0)
			return;
		mBuffer->Upload();

		mCircleMaterial->Submit();

		renderer.DrawElements(ETopologyMode::Triangles, mBuffer->GetVAO(), mBuffer->GetIndexCount());

		mIsActive = false;
	}

	bool CircleRenderBatch::IsFull(uint32_t vNeeded, uint32_t iNeeded)
	{
		return (mBuffer->GetVertexCount() + vNeeded > sMaxVertexCount) || (mBuffer->GetIndexCount() + iNeeded > sMaxIndexCount);
	}
} // namespace BHive