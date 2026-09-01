#include "QuadRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{
#define QUAD_PIPELINE_NAME "QuadPipeline"

	void QuadRenderBatch::Initialize()
	{
		mBuffer = CreateScope<VertexBatchBuffer<QuadVertex>>(sMaxVertexCount, sMaxIndexCount, true);

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;

		mQuadMaterial = MaterialFactory::Create("Quad.glsl");
		mPipeline = PipelineFactory::Create(state);
	}

	void QuadRenderBatch::Flush(Renderer &renderer)
	{
		if (mBuffer->GetIndexCount() == 0)
			return;

		mBuffer->Upload();

		auto texture = mTextureBatch->GetTexture();
		mQuadMaterial.As<Material>()->SetTexture("uTexture", {texture});

		auto &pass = renderer.GetActivePass();
		pass.Emplace<CmdBindPipeline>()(mPipeline);
		pass.Emplace<CmdBindMaterial>()(mQuadMaterial.As<Material>());
		pass.Emplace<CmdDrawIndexed>()(ETopologyMode::Triangles, mBuffer->GetVAO(), mBuffer->GetIndexCount());

		mIsActive = false;
	}

	bool QuadRenderBatch::NeedsFlush(uint32_t vNeeded, uint32_t iNeeded)
	{
		return IsFull(vNeeded, iNeeded);
	}

	void QuadRenderBatch::StartBatch()
	{
		mBuffer->Reset();
		mIsActive = true;
	}

	void QuadRenderBatch::SetTextureBatch(TextureBatchData *texture_batch)
	{
		mTextureBatch = texture_batch;
	}

	bool QuadRenderBatch::IsFull(uint32_t vNeeded, uint32_t iNeeded)
	{
		return (mBuffer->GetIndexCount() + iNeeded) > sMaxIndexCount || (mBuffer->GetVertexCount() + vNeeded) > sMaxVertexCount;
	}

} // namespace BHive