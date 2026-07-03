#include "BloomMaterial.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	Ref<Texture> BloomMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto bloomOutput = allocator.GetBloomOutput();
		auto baseSize = bloomOutput->GetSize();
		auto compositeOutput = allocator.GetBloomCompositeOutput();

		auto params = Params;
		auto mipCount = allocator.GetBloomMipCount();
		

		auto &pass = graph.AddPass("Bloom", EPassType::OffScreen);

		// Phase 0 : Prefilter Scene color
		{
			auto prefilterBindings = FComputeBindings(PipelineRegistry::Get("BLOOM_PREFILTER"))
				.Set({"uSceneColor", input})
				.Set({"uOutput", bloomOutput})
				.Set<float>({"uThreshold", Params.Threshold});

			auto dstSize = bloomOutput->GetSize();

			pass.BeginPhase();
			pass.Push(input, EImageAccess::ComputeSampled);
			pass.Push(bloomOutput, EImageAccess::ComputeStorageWrite);
			pass.Emplace<CmdBindMaterial>()(&prefilterBindings);
			pass.Emplace<CmdDisptach>()((dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1);
			pass.EndPhase();
		}

		// Phase 1: Downsample
		{
			
			glm::uvec2 mipSize = bloomOutput->GetSize();

			auto downsampleBindings = FComputeBindings(PipelineRegistry::Get("BLOOM_DOWNSAMPLE"));
			

			for (uint32_t mip = 0; mip < mipCount - 1; mip++)
			{
				uint32_t srcMip = mip;
				uint32_t dstMip = mip + 1;

				glm::uvec2 dstSize = glm::max(mipSize / 2u, glm::uvec2(1u));

				downsampleBindings.Set({"uSrcTexture", bloomOutput, {srcMip, 1, 0, 1}});
				downsampleBindings.Set({"uOutput", bloomOutput, {dstMip, 1, 0, 1}});

				pass.BeginPhase();
				pass.Push(bloomOutput, EImageAccess::ComputeStorageRead, {srcMip, 1, 0, 1});
				pass.Push(bloomOutput, EImageAccess::ComputeStorageWrite, {dstMip, 1, 0, 1});
				pass.Emplace<CmdBindMaterial>()(&downsampleBindings);
				pass.Emplace<CmdDisptach>()((dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1);
				pass.EndPhase();

				mipSize = dstSize;
			}
		}
		
		//Phase 2: UpSample
		auto upsampleBindings = FComputeBindings(PipelineRegistry::Get("BLOOM_UPSAMPLE")).Set<float>({"uFilterRadius", Params.Radius});

		for (uint32_t mip = mipCount - 1; mip > 0; mip--)
		{
			uint32_t srcMip = mip;
			uint32_t dstMip = mip - 1;

			glm::uvec2 dstSize = glm::max(baseSize >> dstMip, glm::uvec2(1u));

			upsampleBindings.Set({"uSrcTexture", bloomOutput, {srcMip, 1, 0, 1}})
				.Set({"uOutput", bloomOutput, {dstMip, 1, 0, 1}});

			pass.BeginPhase();
			pass.Push(bloomOutput, EImageAccess::ComputeStorageRead, {srcMip, 1, 0, 1});
			pass.Push(bloomOutput, EImageAccess::ComputeStorageWrite, {dstMip, 1, 0, 1});
			pass.Emplace<CmdBindMaterial>()(&upsampleBindings);
			pass.Emplace<CmdDisptach>()((dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1);
			pass.EndPhase();
		}

		glm::uvec2 dstSize = compositeOutput->GetSize();

		auto compositeBindings = FComputeBindings(PipelineRegistry::Get("BLOOM_COMBINE"))
									 .Set({"uTextureA", input})
									 .Set({"uTextureB", bloomOutput})
									 .Set({"uOutput", compositeOutput})
									 .Set<float>({"uExposure", Params.Exposure})
									 .Set<float>({"uBloomStrength", Params.Strength});
		

		//Phase 3 : compostie scene and bloom
		pass.BeginPhase();

		pass.Push(input, EImageAccess::ComputeSampled);
		pass.Push(bloomOutput, EImageAccess::ComputeSampled);
		pass.Push(compositeOutput, EImageAccess::ComputeStorageWrite);
		pass.Emplace<CmdBindMaterial>()(&compositeBindings);
		pass.Emplace<CmdDisptach>()((dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1);

		pass.BeginPhase();
		pass.Push(compositeOutput, EImageAccess::ColorRead);
		pass.EndPhase();
		
		return compositeOutput;
	}
} // namespace BHive