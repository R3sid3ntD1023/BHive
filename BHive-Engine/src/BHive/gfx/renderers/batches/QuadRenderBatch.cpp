#include "QuadRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/material/Material.h"
#include "gfx/Pipeline.h"

namespace BHive
{
#define QUAD_PIPELINE_NAME "QuadPipeline"

	void QuadRenderBatch::Initialize()
	{
		mBuffer = CreateScope<VertexBatchBuffer<QuadVertex>>(sMaxVertexCount, sMaxIndexCount, true);

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;

		PipelineRegistry::Register(QUAD_PIPELINE_NAME, state);

		auto shader = ShaderManager::Get("Quad.glsl");
		mQuadMaterial = CreateScope<Material>(shader);
	}

	void QuadRenderBatch::Flush(Renderer &renderer)
	{
		if (mBuffer->GetIndexCount() == 0)
			return;

		mBuffer->Upload();

		auto &texture = mTextureBatch->GetTexture();
		mQuadMaterial->SetTexture("uTexture", {texture});

		auto &pass = renderer.GetActivePass();
		pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get(QUAD_PIPELINE_NAME));
		pass.Emplace<CmdBindMaterial>()(mQuadMaterial.get());
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