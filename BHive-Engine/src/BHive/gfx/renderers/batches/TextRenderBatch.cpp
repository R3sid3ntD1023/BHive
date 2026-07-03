#include "TextRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/material/Material.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	#define PIPELINE_NAME "TextPipeline"

	void TextRenderBatch::Initialize()
	{
		mBuffer = CreateScope<VertexBatchBuffer<TextVertex>>(sMaxVertexCount, sMaxIndexCount, true);

		auto shader = ShaderManager::Get("Text.glsl");

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.ShaderProgram = shader;
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;

		PipelineRegistry::Register(PIPELINE_NAME, state);

		mMaterial = CreateScope<Material>();
		mMaterial->SetPipeline(PipelineRegistry::Get(PIPELINE_NAME));
	}

	bool TextRenderBatch::NeedsFlush(uint32_t vNeeded, uint32_t iNeeded)
	{
		return IsFull(vNeeded, iNeeded);
	}

	void TextRenderBatch::StartBatch()
	{
		mBuffer->Reset();
		mIsActive = true;
	}

	void TextRenderBatch::Flush(Renderer &renderer)
	{
		if (mBuffer->GetIndexCount() == 0)
			return;

		mBuffer->Upload();

		auto &texture = mTextureBatch->GetTexture();
		mMaterial->SetTexture("uTexture", texture, 0);
		mMaterial->Submit();

		auto &pass = renderer.GetActivePass();
		pass.Emplace<CmdBindMaterial>()(mMaterial.get());
		pass.Emplace<CmdDrawIndexed>()(ETopologyMode::Triangles, mBuffer->GetVAO(), mBuffer->GetIndexCount());

		mIsActive = false;
	}

	void TextRenderBatch::SetTextureBatch(TextureBatchData *texture_batch)
	{
		mTextureBatch = texture_batch;
	}

	bool TextRenderBatch::IsFull(uint32_t vNeeded, uint32_t iNeeded)
	{
		return (mBuffer->GetIndexCount() + iNeeded) > sMaxIndexCount || (mBuffer->GetVertexCount() + vNeeded) > sMaxVertexCount;
	}
} // namespace BHive