#include "AcesMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	Ref<Texture> AcesMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto output = allocator.GetAcesOutput();
		auto dstSize = output->GetSize();

		if (!mFramebuffer || (mFramebuffer && (mFramebuffer->GetSize() != dstSize)))
		{
			FFramebufferTexture color{};
			color.ExternalTexture = output;

			FramebufferSpecification spec{};
			spec.DebugName = "Aces";
			spec.Size = dstSize;
			spec.Attachments.AddColorAttachment(color);
			mFramebuffer = Framebuffer::Create(spec);
		}

		auto bindings = FComputeBindings(PipelineRegistry::Get("ACES")).Set({"uSceneColor", input});

		auto &pass = graph.AddPass("Aces", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Graphics);
		pass.Push(mFramebuffer);
		pass.Push(input, EImageAccess::ColorRead);
		pass.Emplace<CmdBindMaterial>()(&bindings);
		pass.Emplace<CmdDrawFullScreen>()();
		pass.EndPhase();

		pass.BeginPhase(EPhaseType::Transfer);
		pass.Push(output, EImageAccess::ColorRead);
		pass.EndPhase();

		return output;
	}

} // namespace BHive