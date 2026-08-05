#include "BloomMaterial.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"

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
			auto prefilterBindings = FComputeBindings(ShaderManager::Get("PreFilter.glsl"));

			prefilterBindings.SetTexture("uSceneColor", FTextureBinding(input)).SetParam("uThreshold", MaterialParam(Params.Threshold));

			pass.BeginPhase(EPhaseType::Graphics);
			pass.Push(mFramebuffers[0]);
			pass.Push(input, EImageAccess::ColorRead);
			pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
			pass.Emplace<CmdBindMaterial>()(&prefilterBindings);
			pass.Emplace<CmdDrawFullScreen>()();
			pass.EndPhase();
		}

		// Phase 1: Downsample
		{

			auto downsampleBindings = FComputeBindings(ShaderManager::Get("DownSample.glsl"));

			for (uint32_t mip = 0; mip < mipCount - 1; mip++)
			{
				uint32_t srcMip = mip;
				uint32_t dstMip = mip + 1;

				ImageSubresourceRange srcRange = {srcMip, 1, 0, 1};
				ImageSubresourceRange dstRange = {dstMip, 1, 0, 1};

				downsampleBindings.SetTexture("uSrcTexture", FTextureBinding(bloomOutput, srcMip));

				pass.BeginPhase(EPhaseType::Graphics);
				pass.Push(mFramebuffers[0], dstRange);
				pass.Push(bloomOutput, EImageAccess::ColorRead, srcRange);
				pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
				pass.Emplace<CmdBindMaterial>()(&downsampleBindings);
				pass.Emplace<CmdDrawFullScreen>()();
				pass.EndPhase();
			}
		}

		// Phase 2: UpSample
		{
			auto upsampleBindings = FComputeBindings(ShaderManager::Get("UpSample.glsl"));

			upsampleBindings.SetParam("uFilterRadius", MaterialParam(Params.Radius));

			for (uint32_t mip = mipCount - 1; mip > 0; mip--)
			{
				uint32_t srcMip = mip;
				uint32_t dstMip = mip - 1;

				ImageSubresourceRange srcRange{srcMip, 1, 0, 1};
				ImageSubresourceRange dstRange{dstMip, 1, 0, 1};

				upsampleBindings.SetTexture("uSrcTexture", FTextureBinding(bloomOutput, srcMip));

				pass.BeginPhase(EPhaseType::Graphics);
				pass.Push(mFramebuffers[0], dstRange);
				pass.Push(bloomOutput, EImageAccess::ColorRead, srcRange);
				pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
				pass.Emplace<CmdBindMaterial>()(&upsampleBindings);
				pass.Emplace<CmdDrawFullScreen>()();
				pass.EndPhase();
			}
		}

		// Composite to scene
		{
			auto compositeBindings = FComputeBindings(ShaderManager::Get("Composite.glsl"));
			compositeBindings.SetTexture("uTextureA", FTextureBinding(input))
				.SetTexture("uTextureB", FTextureBinding(bloomOutput))
				.SetParam("uExposure", MaterialParam(Params.Exposure))
				.SetParam("uBloomStrength", MaterialParam(Params.Strength));

			// Phase 3 : composite scene and bloom
			pass.BeginPhase(EPhaseType::Graphics);
			pass.Push(mFramebuffers[1]);
			pass.Push(input, EImageAccess::ColorRead);
			pass.Push(bloomOutput, EImageAccess::ColorRead);
			pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
			pass.Emplace<CmdBindMaterial>()(&compositeBindings);
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