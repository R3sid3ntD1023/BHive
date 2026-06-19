#include "BloomMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/ShaderManager.h"

namespace BHive
{
	BloomMaterial::BloomMaterial()
	{
		{
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = ShaderManager::Get("CombineTex.glsl");
			PipelineRegistry::Register("BLOOM_COMBINE", state);
		}

		{
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = ShaderManager::Get("PreFilter.glsl");
			PipelineRegistry::Register("BLOOM_PREFILTER", state);
		}

		{
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = ShaderManager::Get("DownSample.glsl");
			PipelineRegistry::Register("BLOOM_DOWNSAMPLE", state);
		}

		{
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = ShaderManager::Get("UpSample.glsl");
			PipelineRegistry::Register("BLOOM_UPSAMPLE", state);
		}
	}

	Ref<Texture> BloomMaterial::AddToGraph(RenderGraph &graph, const Ref<Texture> &input)
	{
		auto &pass = graph.AddPass("Bloom", EPassType::OffScreen);
	
		auto params = Params;
		
		pass.CommandList.Push(
			"Prefilter Scene Color",
			[input, bloom = mBloomTex, params](IRendererContext &ctx)
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
			[bloom = mBloomTex, mipCount = mMipCount](IRendererContext &ctx)
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
							b.Set("uSrcMip", (int32_t)srcMip);
						});

					mipSize = dstSize;	
				}
			});

		pass.CommandList.Push(
			"UpSample Output",
			[bloom = mBloomTex, mipCount = mMipCount, params](IRendererContext &ctx)
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
			[scene = input, bloom = mBloomTex, output = mOutputTex, mipCount = mMipCount](IRendererContext &ctx)
			{
				glm::uvec2 dstSize = output->GetSize();
				glm::uvec3 dispatch = {dstSize, 1};

				Renderer::Get().ExecuteComputePass(
					PipelineRegistry::Get("BLOOM_COMBINE"), dispatch,
					[scene, bloom, output, mipCount](FComputeBindings &b)
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
					});
			});

		return mOutputTex;
	}

	void BloomMaterial::CreateResizableObjects(const glm::uvec2 &size)
	{
		mMipCount = std::min(ComputeMipCount(size), 5u);

		{
			FTextureCreateInfo info{};
			info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			info.Format = EFormat::RGBA32F;
			info.Roles |= ETextureRole::ComputeWrite;
			info.DebugName = "SceneBloomCombined";
			mOutputTex = Texture2D::Create(size, info);
		}

		{
			FTextureCreateInfo info{};
			info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			info.Format = EFormat::RGBA32F;
			info.Roles |= ETextureRole::ComputeWrite;
			info.MipLevels = mMipCount;
			info.DebugName = "BloomMipChain";

			mBloomTex = Texture2D::Create(size, info);
		}
	}

	uint32_t BloomMaterial::ComputeMipCount(glm::uvec2 size)
	{
		uint32_t levels = 1;
		while (size.x > 1 || size.y > 1)
		{
			size = glm::max(size / 2u, glm::uvec2(1u));
			levels++;
		}

		return levels;
	}
} // namespace BHive