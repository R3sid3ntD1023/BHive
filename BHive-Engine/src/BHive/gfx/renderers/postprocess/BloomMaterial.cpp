#include "BloomMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/ShaderManager.h"

namespace BHive
{
	Ref<Texture> BloomMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		mAllocator = &allocator;
		mInput = input;
		mBloomOutput = allocator.GetBloomOuput();
		mCompositeOutput = allocator.GetBloomCompositeOuput();

		auto params = Params;
		auto mipCount = allocator.GetBloomMipCount();
	
		auto &pass = graph.AddPass("Bloom", EPassType::OffScreen);

		//Phase 0 : Prefilter Scene color
		pass.BeginPhase();
		pass.Push(input, EImageAccess::ComputeSampled);
		pass.Push(mBloomOutput, EImageAccess::ComputeStorageWrite);
		pass.Push("Prefilter Scene Color", this, &BloomMaterial::DoPrefilterSceneColor);
		pass.EndPhase();

		//Phase 1: Downsample
		glm::uvec2 mipSize = mBloomOutput->GetSize();

		for (uint32_t mip = 0; mip < mipCount - 1; mip++)
		{
			uint32_t srcMip = mip;
			uint32_t dstMip = mip + 1;

			glm::uvec2 dstSize = glm::max(mipSize / 2u, glm::uvec2(1u));

			pass.BeginPhase();
			pass.Push(mBloomOutput, EImageAccess::ComputeStorageRead, {srcMip, 1, 0, 1});
			pass.Push(mBloomOutput, EImageAccess::ComputeStorageWrite, {dstMip, 1, 0, 1});
			pass.Push("Compute Downsample", this, &BloomMaterial::DoDownSample, srcMip, dstMip);
			pass.EndPhase();

			mipSize = dstSize;
		}
		
		//Phase 2: UpSample

		for (uint32_t mip = mipCount - 1; mip > 0; mip--)
		{
			uint32_t srcMip = mip;
			uint32_t dstMip = mip - 1;

			pass.BeginPhase();
			pass.Push(mBloomOutput, EImageAccess::ComputeStorageRead, {srcMip, 1, 0, 1});
			pass.Push(mBloomOutput, EImageAccess::ComputeStorageWrite, {dstMip, 1, 0, 1});
			pass.Push("Compute Downsample", this, &BloomMaterial::DoUpSample, mipCount, srcMip, dstMip);
			pass.EndPhase();
		}

		//Phase 3 : compostie scene and bloom
		pass.BeginPhase();

		pass.Push(mInput, EImageAccess::ComputeSampled);
		pass.Push(mBloomOutput, EImageAccess::ComputeSampled);
		pass.Push(mCompositeOutput, EImageAccess::ComputeStorageWrite);

		pass.Push("Combine Bloom and Scene", this, &BloomMaterial::DoComposite, mipCount);
		
		return mCompositeOutput;
	}

	void BloomMaterial::DoPrefilterSceneColor(IRendererContext &ctx)
	{
		auto dstSize = mBloomOutput->GetSize();
		glm::uvec3 dispatch = {(dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1};

		// prefilter
		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get("BLOOM_PREFILTER"), dispatch,
			[this](FComputeBindings &b)
			{
				b.Bind("uSceneColor", mInput);
				b.Bind("uOutput", mBloomOutput);
				b.Set("uThreshold", Params.Threshold);
			});
	}

	void BloomMaterial::DoDownSample(IRendererContext &ctx, uint32_t srcMip, uint32_t dstMip)
	{
		auto dstSize = mAllocator->GetBloomMipSize(dstMip);
		glm::uvec3 dispatch = {(dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1};

		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get("BLOOM_DOWNSAMPLE"), dispatch,
			[this, srcMip, dstMip](FComputeBindings &b)
			{
				b.Bind("uSrcTexture", mBloomOutput, {srcMip, 1, 0, 1});
				b.Bind("uOutput", mBloomOutput, {dstMip, 1, 0, 1});
			});
	}

	void BloomMaterial::DoUpSample(IRendererContext &ctx, uint32_t mipCount, uint32_t srcMip, uint32_t dstMip)
	{
		glm::uvec2 baseSize = mAllocator->GetBloomOuput()->GetSize();
		glm::uvec2 dstSize = glm::max(baseSize >> dstMip, glm::uvec2(1u));
		glm::uvec3 dispatch = {(dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1};

		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get("BLOOM_UPSAMPLE"), dispatch,
			[this, mipCount, srcMip, dstMip](FComputeBindings &b)
			{
				b.Bind("uSrcTexture", mBloomOutput, {srcMip, 1, 0, 1});
				b.Bind("uOutput", mBloomOutput, {dstMip, 1, 0, 1});
				b.Set("uFilterRadius", Params.Radius);
			});
	}

	void BloomMaterial::DoComposite(IRendererContext &ctx, uint32_t mipCount)
	{
		glm::uvec2 dstSize = mCompositeOutput->GetSize();
		glm::uvec3 dispatch = {(dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1};

		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get("BLOOM_COMBINE"), dispatch,
			[this, mipCount](FComputeBindings &b)
			{

				b.Bind("uTextureA", mInput);
				b.Bind("uTextureB", mBloomOutput);
				b.Bind("uOutput", mCompositeOutput);
				b.Set("uExposure", Params.Exposure);
				b.Set("uBloomStrength", Params.Strength);
			});
	}

} // namespace BHive