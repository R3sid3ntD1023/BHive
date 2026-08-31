#include "AcesMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"
#include "gfx/factories/GFXFactories.h"

namespace BHive
{
	AcesMaterial::AcesMaterial()
	{
		mMaterial = MaterialFactory::Create("Aces.glsl");
	}

	TexturePtr AcesMaterial::AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set)
	{
		auto output = mFramebuffer.As<Framebuffer>()->GetColorAttachment();
		auto input = set.PrevOutput;

		auto material = mMaterial.As<Material>();
		material->SetTexture("uSceneColor", FTextureBinding(input));

		auto &pass = graph.AddPass("Aces", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Graphics);
		pass.UseFramebuffer(mFramebuffer);
		pass.UseTexture(input, EImageUsage::ColorRead);
		pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
		pass.Emplace<CmdBindMaterial>()(material);
		pass.Emplace<CmdDrawFullScreen>()();
		pass.EndPhase();

		pass.BeginPhase(EPhaseType::Transfer);
		pass.UseTexture(output, EImageUsage::ColorRead);
		pass.EndPhase();

		return output;
	}

	void AcesMaterial::Init(const glm::uvec2 &size)
	{
		FTextureCreateInfo info{};
		info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		info.Format = EFormat::RGBA8;
		info.Roles |= ETextureRole::RenderTarget;
		info.DebugName = "AcesOutput";

		FFramebufferTexture color{info, ETextureType::TEXTURE_2D};

		FramebufferSpecification spec{};
		spec.DebugName = "Aces";
		spec.Size = size;
		spec.Attachments.AddColorAttachment(color);
		mFramebuffer = FramebufferFactory::Create(spec);
	}

} // namespace BHive