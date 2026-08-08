#include "BloomMaterial.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	BloomMaterial::BloomMaterial()
	{
		mMaterials[0] = CreateScope<Material>(ShaderManager::Get("PreFilter.glsl"));
		mMaterials[1] = CreateScope<Material>(ShaderManager::Get("DownSample.glsl"));
		mMaterials[2] = CreateScope<Material>(ShaderManager::Get("UpSample.glsl"));
		mMaterials[3] = CreateScope<Material>(ShaderManager::Get("Composite.glsl"));
	}

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
			mMaterials[0]->SetTexture("uSceneColor", FTextureBinding(input)).SetParam("uThreshold", MaterialParam(Params.Threshold));

			pass.BeginPhase(EPhaseType::Graphics);
			pass.Push(mFramebuffers[0]);
			pass.Push(input, EImageAccess::ColorRead);
			pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
			pass.Emplace<CmdBindMaterial>()(mMaterials[0].get());
			pass.Emplace<CmdDrawFullScreen>()();
			pass.EndPhase();
		}

		// Phase 1: Downsample
		{
			for (uint32_t mip = 0; mip < mipCount - 1; mip++)
			{
				uint32_t srcMip = mip;
				uint32_t dstMip = mip + 1;

				ImageSubresourceRange srcRange = {srcMip, 1, 0, 1};
				ImageSubresourceRange dstRange = {dstMip, 1, 0, 1};

				mMaterials[1]->SetTexture("uSrcTexture", FTextureBinding(bloomOutput, srcMip));

				pass.BeginPhase(EPhaseType::Graphics);
				pass.Push(mFramebuffers[0], dstRange);
				pass.Push(bloomOutput, EImageAccess::ColorRead, srcRange);
				pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
				pass.Emplace<CmdBindMaterial>()(mMaterials[1].get());
				pass.Emplace<CmdDrawFullScreen>()();
				pass.EndPhase();
			}
		}

		// Phase 2: UpSample
		{
			mMaterials[2]->SetParam("uFilterRadius", MaterialParam(Params.Radius));

			for (uint32_t mip = mipCount - 1; mip > 0; mip--)
			{
				uint32_t srcMip = mip;
				uint32_t dstMip = mip - 1;

				ImageSubresourceRange srcRange{srcMip, 1, 0, 1};
				ImageSubresourceRange dstRange{dstMip, 1, 0, 1};

				mMaterials[2]->SetTexture("uSrcTexture", FTextureBinding(bloomOutput, srcMip));

				pass.BeginPhase(EPhaseType::Graphics);
				pass.Push(mFramebuffers[0], dstRange);
				pass.Push(bloomOutput, EImageAccess::ColorRead, srcRange);
				pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
				pass.Emplace<CmdBindMaterial>()(mMaterials[2].get());
				pass.Emplace<CmdDrawFullScreen>()();
				pass.EndPhase();
			}
		}

		// Composite to scene
		{
			mMaterials[3]->SetTexture("uTextureA", FTextureBinding(input));
			mMaterials[3]->SetTexture("uTextureB", FTextureBinding(bloomOutput));
			mMaterials[3]->SetParam("uExposure", MaterialParam(Params.Exposure));
			mMaterials[3]->SetParam("uBloomStrength", MaterialParam(Params.Strength));

			// Phase 3 : composite scene and bloom
			pass.BeginPhase(EPhaseType::Graphics);
			pass.Push(mFramebuffers[1]);
			pass.Push(input, EImageAccess::ColorRead);
			pass.Push(bloomOutput, EImageAccess::ColorRead);
			pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
			pass.Emplace<CmdBindMaterial>()(mMaterials[3].get());
			pass.Emplace<CmdDrawFullScreen>()();

			pass.BeginPhase(EPhaseType::Transfer);
			pass.Push(compositeOutput, EImageAccess::ColorRead);
			pass.EndPhase();
		}

		return compositeOutput;
	}

	void BloomMaterial::OnResize(const glm::uvec2 &size, PostProcessAllocator &allocator)
	{
		auto bloomOutput = allocator.GetBloomOutput();
		auto compositeOutput = allocator.GetBloomCompositeOutput();
		auto textures = std::vector{bloomOutput, compositeOutput};

		for (uint32_t i = 0; i < 2; i++)
		{
			auto &fbo = mFramebuffers[i];
			auto &target = textures[i];
			auto targetSize = target->GetSize();
			if (!fbo || (fbo && (fbo->GetSize() != targetSize)))
			{
				FFramebufferTexture color{};
				color.ExternalTexture = target;

				FramebufferSpecification spec{};
				spec.DebugName = target->GetInfo().DebugName;
				spec.Size = targetSize;
				spec.Attachments.AddColorAttachment(color);
				fbo = Framebuffer::Create(spec);
			}
		}
	}
} // namespace BHive