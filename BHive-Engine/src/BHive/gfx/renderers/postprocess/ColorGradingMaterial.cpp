#include "ColorGradingMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	Ref<Texture> ColorGradingMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto output = allocator.GetColorGradeOutput();
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

		auto bindings = FComputeBindings(PipelineRegistry::Get("COLOR_GRADING"))
							.Set({"uTonemapped", input})
							.Set<glm::vec3>({"uLift", Params.Lift})
							.Set<glm::vec3>({"uGamma", Params.Gamma})
							.Set<glm::vec3>({"uGain", Params.Gain})
							.Set<float>({"uSaturation", Params.Saturation});

		auto &pass = graph.AddPass("Color Grading Pass", EPassType::OffScreen);
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