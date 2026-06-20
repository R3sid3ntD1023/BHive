#include "BloomMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/ShaderManager.h"

namespace BHive
{
	Ref<Texture> BloomMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto &pass = graph.AddPass("Bloom", EPassType::OffScreen);
	
		auto params = Params;
		auto mipCount = allocator.GetBloomMipCount();
		auto bloom = allocator.GetBloomOuput();
		auto output = allocator.GetBloomColorOuput();

		pass.CommandList.Push(
			"Prefilter Scene Color",
			[input, bloom, params](IRendererContext &ctx)
			{
				auto dstSize = bloom->GetSize();
				glm::uvec3 dispatch = {dstSize, 1};

				// prefilter
				Renderer::Get().ExecuteComputePass(
					PipelineRegistry::Get("BLOOM_PREFILTER"), dispatch,
					[input, bloom, params](FComputeBindings &b)
					{
						FImageInfo in{};
						in.Texture = input;
						in.Access = EImageAccess::READ;

						FImageInfo out{};
						out.Texture = bloom;
						out.Access = EImageAccess::WRITE;

						b.SampledImage("uSceneColor", in);
						b.StorageImage("uOutput", out);
						b.Set("uThreshold", params.Threshold);
					});
			});



		pass.CommandList.Push(
			"DownSample Prefiter",
			[bloom, mipCount](IRendererContext &ctx)
			{
				glm::uvec2 mipSize = bloom->GetSize();
				
				for (uint32_t mip = 0; mip < mipCount - 1; mip++)
				{
					uint32_t srcMip = mip;
					uint32_t dstMip = mip + 1;

					glm::uvec2 dstSize = glm::max(mipSize / 2u, glm::uvec2(1u));
					glm::uvec3 dispatch = {dstSize, 1};

					Renderer::Get().ExecuteComputePass(
						PipelineRegistry::Get("BLOOM_DOWNSAMPLE"), dispatch,
						[bloom, mipCount, srcMip, dstMip](FComputeBindings &b)
						{								
							FImageInfo in{};
							in.Texture = bloom;
							in.BaseMip = srcMip; //sampler sees full chain
							in.LevelCount = 1;
							in.Access = EImageAccess::READ;

							FImageInfo out{};
							out.Texture = bloom;
							out.BaseMip = dstMip;
							out.LayerCount = 1;
							out.Access = EImageAccess::WRITE;

							b.SampledImage("uSrcTexture", in);
							b.StorageImage("uOutput", out);
						});

					mipSize = dstSize;	
				}
			});

		pass.CommandList.Push(
			"UpSample Output",
			[bloom, mipCount, params](IRendererContext &ctx)
			{
				glm::uvec2 baseSize = bloom->GetSize();

				for (uint32_t mip = mipCount - 1; mip > 0; mip--)
				{
					uint32_t srcMip = mip;
					uint32_t dstMip = mip - 1;

					glm::uvec2 dstSize = glm::max(baseSize >> dstMip, glm::uvec2(1u));
					glm::uvec3 dispatch = {dstSize, 1};

					Renderer::Get().ExecuteComputePass(
						PipelineRegistry::Get("BLOOM_UPSAMPLE"), dispatch,
						[bloom, mipCount, srcMip, dstMip, params](FComputeBindings &b)
						{
							FImageInfo in{};
							in.Texture = bloom;
							in.BaseMip = srcMip;
							in.LevelCount = 1;
							in.Access = EImageAccess::READ;

							FImageInfo out{};
							out.Texture = bloom;
							out.BaseMip = dstMip;
							out.LayerCount = 1;
							out.Access = EImageAccess::WRITE;

							b.SampledImage("uSrcTexture", in);
							b.StorageImage("uOutput", out);
							b.Set("uFilterRadius", params.Radius);
							b.Set("uSrcMip", (int32_t)srcMip);
						});
				}
			});

		pass.CommandList.Push(
			"Combine Bloom and Scene",
			[scene = input, bloom, output, mipCount , params](IRendererContext &ctx)
			{
				glm::uvec2 dstSize = output->GetSize();
				glm::uvec3 dispatch = {dstSize, 1};

				Renderer::Get().ExecuteComputePass(
					PipelineRegistry::Get("BLOOM_COMBINE"), dispatch,
					[scene, bloom, output, mipCount, params](FComputeBindings &b)
					{
						FImageInfo inA{};
						inA.Texture = scene;
						inA.Access = EImageAccess::READ;

						FImageInfo inB{};
						inB.Texture = bloom;
						inB.Access = EImageAccess::READ;

						FImageInfo out{};
						out.Texture = output;
						out.Access = EImageAccess::WRITE;

						b.SampledImage("uTextureA", inA);
						b.SampledImage("uTextureB", inB);
						b.StorageImage("uOutput", out);
						b.Set("uExposure", params.Exposure);
						b.Set("uBloomStrength", params.Strength);
					});
			});

		return output;
	}

} // namespace BHive