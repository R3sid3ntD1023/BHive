#include "AcesMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	AcesMaterial::AcesMaterial()
	{
		mMaterial = CreateScope<Material>(ShaderManager::Get("Aces.glsl"));
	}

	Ref<Texture> AcesMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto output = allocator.GetAcesOutput();
		auto dstSize = output->GetSize();

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

	void AcesMaterial::OnResize(const glm::uvec2 &size, PostProcessAllocator &allocator)
	{
		auto output = allocator.GetAcesOutput();
		auto dstSize = output->GetSize();

		if (!mFramebuffer || mFramebuffer->GetSize() != dstSize)
		{
			FFramebufferTexture color{};
			color.ExternalTexture = output;

			FramebufferSpecification spec{};
			spec.DebugName = "Aces";
			spec.Size = dstSize;
			spec.Attachments.AddColorAttachment(color);
			mFramebuffer = Framebuffer::Create(spec);
		}
	}

} // namespace BHive