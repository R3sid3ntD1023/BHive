#include "BloomMaterial.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	BloomMaterial::BloomMaterial()
	{
		mMaterials[0] = CreateScope<Material>("PreFilter.glsl");
		mMaterials[1] = CreateScope<Material>("DownSample.glsl");
		mMaterials[2] = CreateScope<Material>("UpSample.glsl");
		mMaterials[3] = CreateScope<Material>("Composite.glsl");
	}

	Ref<Texture> BloomMaterial::AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set)
	{
		auto bloomOutput = mFramebuffers[0]->GetColorAttachment();
		auto compositeOutput = mFramebuffers[1]->GetColorAttachment();
		auto baseSize = bloomOutput->GetSize();
		auto params = Params;
		auto input = set.SceneColor;
		uint32_t mipCount = mMipSizes.size();

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

	void BloomMaterial::Init(const glm::uvec2 &size)
	{
		std::array<FTextureCreateInfo, 2> infos{FTextureCreateInfo{}, FTextureCreateInfo{}};

		glm::uvec2 halfSize = glm::max(size / 2u, glm::uvec2(1u));

		// calculate mip sizes
		{
			mMipSizes.clear();

			glm::uvec2 mipSize = halfSize;

			for (uint32_t i = 0; i < MipCount; i++)
			{
				mMipSizes.emplace_back(mipSize);
				mipSize = glm::max(mipSize / 2u, glm::uvec2(1u));
			}
		}

		infos[0].WrapMode = EWrapMode::CLAMP_TO_EDGE;
		infos[0].Format = EFormat::RGBA32F;
		infos[0].Roles |= ETextureRole::RenderTarget;
		infos[0].MipLevels = std::min(ComputeMipCount(halfSize), MipCount);
		infos[0].DebugName = "BloomMipChain";

		infos[1].WrapMode = EWrapMode::CLAMP_TO_EDGE;
		infos[1].Format = EFormat::RGBA32F;
		infos[1].Roles |= ETextureRole::RenderTarget;
		infos[1].DebugName = "SceneBloomComposite";

		std::array<glm::uvec2, 2> sizes{halfSize, size};

		for (uint32_t i = 0; i < 2; i++)
		{
			auto &fbo = mFramebuffers[i];
			FFramebufferTexture color{infos[i], ETextureType::TEXTURE_2D};

			FramebufferSpecification spec{};
			spec.DebugName = "Bloom_";
			spec.Size = sizes[i];
			spec.Attachments.AddColorAttachment(color);
			fbo = Framebuffer::Create(spec);
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

	REFLECT(BloomMaterial::FParams)
	{
		BEGIN_REFLECT(BloomMaterial::FParams)
		REFLECT_PROPERTY(Threshold)
		REFLECT_PROPERTY(Radius)
		REFLECT_PROPERTY(Strength)
		REFLECT_PROPERTY(Exposure);
	}
} // namespace BHive