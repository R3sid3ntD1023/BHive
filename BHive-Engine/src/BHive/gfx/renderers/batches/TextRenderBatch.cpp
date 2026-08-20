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

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;

		state.Blend.Enabled = true;
		state.Blend.SrcColor = EBlendFactor::SrcAlpha;
		state.Blend.DstColor = EBlendFactor::OneMinusSrcAlpha;
		state.Blend.ColorOp = EBlendOp::Add;

		state.Blend.SrcAlpha = EBlendFactor::One;
		state.Blend.DstAlpha = EBlendFactor::OneMinusSrcAlpha;
		state.Blend.AlphaOp = EBlendOp::Add;

		PipelineRegistry::Register(PIPELINE_NAME, state);

		mMaterial = CreateScope<Material>("Text.glsl");
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
		mMaterial->SetTexture("uTexture", {texture});

		auto &pass = renderer.GetActivePass();
		pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get(PIPELINE_NAME));
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