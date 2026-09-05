#include "BloomMaterial.h"
#include "gfx/Framebuffer.h"
#include "gfx/Pipeline.h"
#include "gfx/factories/GFXFactories.h"
#include "gfx/material/Material.h"

namespace BHive
{
	TexturePtr BloomMaterial::AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set)
	{
		auto bloomOutput = mFramebuffers[0].As<Framebuffer>()->GetColorAttachment();
		auto compositeOutput = mFramebuffers[1].As<Framebuffer>()->GetColorAttachment();
		auto baseSize = bloomOutput.As<Texture>()->GetSize();
		auto params = Params;
		auto input = set.SceneColor;
		uint32_t mipCount = mMipSizes.size();

		auto &pass = graph.AddPass("Bloom", EPassType::OffScreen);

		// Phase 0 : Prefilter Scene color
		{
			auto mat = mMaterials[0].As<Material>();
			mat->SetTexture("uSceneColor", TextureBinding(input)).SetParam("uThreshold", MaterialParam(Params.Threshold));

			pass.BeginPhase(EPhaseType::Graphics);
			pass.UseFramebuffer(mFramebuffers[0]);
			pass.UseTexture(input, EImageUsage::ColorRead);
			pass.Emplace<CmdBindPipeline>()(mPipeline);
			pass.Emplace<CmdBindMaterial>()(mat);
			pass.Emplace<CmdDrawFullScreen>()();
			pass.EndPhase();
		}

		// Phase 1: Downsample
		{
			auto mat = mMaterials[1].As<Material>();

			for (uint32_t mip = 0; mip < mipCount - 1; mip++)
			{
				uint32_t srcMip = mip;
				uint32_t dstMip = mip + 1;

				ImageSubresourceRange srcRange = {srcMip, 1, 0, 1};
				ImageSubresourceRange dstRange = {dstMip, 1, 0, 1};

				mat->SetTexture("uSrcTexture", TextureBinding(bloomOutput, srcMip));

				pass.BeginPhase(EPhaseType::Graphics);
				pass.UseFramebuffer(mFramebuffers[0], dstRange);
				pass.UseTexture(bloomOutput, EImageUsage::ColorRead, srcRange);
				pass.Emplace<CmdBindPipeline>()(mPipeline);
				pass.Emplace<CmdBindMaterial>()(mat);
				pass.Emplace<CmdDrawFullScreen>()();
				pass.EndPhase();
			}
		}

		// Phase 2: UpSample
		{
			auto mat = mMaterials[2].As<Material>();
			mat->SetParam("uFilterRadius", MaterialParam(Params.Radius));

			for (uint32_t mip = mipCount - 1; mip > 0; mip--)
			{
				uint32_t srcMip = mip;
				uint32_t dstMip = mip - 1;

				ImageSubresourceRange srcRange{srcMip, 1, 0, 1};
				ImageSubresourceRange dstRange{dstMip, 1, 0, 1};

				mat->SetTexture("uSrcTexture", TextureBinding(bloomOutput, srcMip));

				pass.BeginPhase(EPhaseType::Graphics);
				pass.UseFramebuffer(mFramebuffers[0], dstRange);
				pass.UseTexture(bloomOutput, EImageUsage::ColorRead, srcRange);
				pass.Emplace<CmdBindPipeline>()(mPipeline);
				pass.Emplace<CmdBindMaterial>()(mat);
				pass.Emplace<CmdDrawFullScreen>()();
				pass.EndPhase();
			}
		}

		// Composite to scene
		{
			auto mat = mMaterials[3].As<Material>();
			mat->SetTexture("uTextureA", TextureBinding(input));
			mat->SetTexture("uTextureB", TextureBinding(bloomOutput));
			mat->SetParam("uExposure", MaterialParam(Params.Exposure));
			mat->SetParam("uBloomStrength", MaterialParam(Params.Strength));

			// Phase 3 : composite scene and bloom
			pass.BeginPhase(EPhaseType::Graphics);
			pass.UseFramebuffer(mFramebuffers[1]);
			pass.UseTexture(input, EImageUsage::ColorRead);
			pass.UseTexture(bloomOutput, EImageUsage::ColorRead);
			pass.Emplace<CmdBindPipeline>()(mPipeline);
			pass.Emplace<CmdBindMaterial>()(mat);
			pass.Emplace<CmdDrawFullScreen>()();

			pass.BeginPhase(EPhaseType::Transfer);
			pass.UseTexture(compositeOutput, EImageUsage::ColorRead);
			pass.EndPhase();
		}

		return compositeOutput;
	}

	void BloomMaterial::Resize(const glm::uvec2 &size)
	{
		if (size.x <= 0 || size.y <= 0)
			return;

		CalculateMipSizes(size);

		mFramebuffers[0].As<Framebuffer>()->Resize(glm::max(size / 2u, glm::uvec2(1u)));
		mFramebuffers[1].As<Framebuffer>()->Resize(size);
	}

	void BloomMaterial::CalculateMipSizes(const glm::uvec2 &size)
	{
		mMipSizes.clear();

		glm::uvec2 mipSize = glm::max(size / 2u, glm::uvec2(1u));

		for (uint32_t i = 0; i < MipCount; i++)
		{
			mMipSizes.emplace_back(mipSize);
			mipSize = glm::max(mipSize / 2u, glm::uvec2(1u));
		}
	}

	void BloomMaterial::Init(const glm::uvec2 &size)
	{
		mMaterials[0] = MaterialFactory::Create("PreFilter.glsl");
		mMaterials[1] = MaterialFactory::Create("DownSample.glsl");
		mMaterials[2] = MaterialFactory::Create("UpSample.glsl");
		mMaterials[3] = MaterialFactory::Create("Composite.glsl");

		mPipeline = PipelineFactory::Create(Pipeline::GetDefaultGraphicsPipelineState());

		std::array<FTextureCreateInfo, 2> infos{FTextureCreateInfo{}, FTextureCreateInfo{}};

		glm::uvec2 halfSize = glm::max(size / 2u, glm::uvec2(1u));

		CalculateMipSizes(size);

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
			fbo = FramebufferFactory::Create(spec);
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