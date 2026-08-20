#include "AcesMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	AcesMaterial::AcesMaterial()
	{
		mMaterial = CreateScope<Material>("Aces.glsl");
	}

	Ref<Texture> AcesMaterial::AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set)
	{
		auto output = mFramebuffer->GetColorAttachment();
		auto input = set.PrevOutput;

		mMaterial->SetTexture("uSceneColor", FTextureBinding(input));

		auto &pass = graph.AddPass("Aces", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Graphics);
		pass.Push(mFramebuffer);
		pass.Push(input, EImageAccess::ColorRead);
		pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
		pass.Emplace<CmdBindMaterial>()(mMaterial.get());
		pass.Emplace<CmdDrawFullScreen>()();
		pass.EndPhase();

		pass.BeginPhase(EPhaseType::Transfer);
		pass.Push(output, EImageAccess::ColorRead);
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
		mFramebuffer = Framebuffer::Create(spec);
	}

} // namespace BHive