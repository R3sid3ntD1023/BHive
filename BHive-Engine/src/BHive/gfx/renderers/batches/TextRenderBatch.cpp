#include "TextRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/renderers/Renderer.h"

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

		mMaterial = MaterialFactory::Create("Text.glsl");
		mPipeline = PipelineFactory::Create(state);
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

		auto texture = mTextureBatch->GetTexture();
		mMaterial.As<Material>()->SetTexture("uTexture", {texture});

		auto &pass = renderer.GetActivePass();
		pass.Emplace<CmdBindPipeline>()(mPipeline);
		pass.Emplace<CmdBindMaterial>()(mMaterial.As<Material>());
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