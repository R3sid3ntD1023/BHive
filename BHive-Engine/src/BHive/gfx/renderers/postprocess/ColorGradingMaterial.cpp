#include "ColorGradingMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	Ref<Texture> ColorGradingMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto output = allocator.GetColorGradeOutput();

		auto bindings = FComputeBindings(ShaderManager::Get("ColorGrading.glsl"));
		bindings.SetTexture("uTonemapped", FTextureBinding(input))
			.SetParam("uLift", MaterialParam(Params.Lift))
			.SetParam("uGamma", MaterialParam(Params.Gamma))
			.SetParam("uGain", MaterialParam(Params.Gain))
			.SetParam("uSaturation", MaterialParam(Params.Saturation));

		auto &pass = graph.AddPass("Color Grading Pass", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Graphics);
		pass.Push(mFramebuffer);
		pass.Push(input, EImageAccess::ColorRead);
		pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
		pass.Emplace<CmdBindMaterial>()(&bindings);
		pass.Emplace<CmdDrawFullScreen>()();
		pass.EndPhase();

		pass.BeginPhase(EPhaseType::Transfer);
		pass.Push(output, EImageAccess::ColorRead);
		pass.EndPhase();

		return output;
	}

	void ColorGradingMaterial::OnResize(const glm::uvec2 &size, PostProcessAllocator &allocator)
	{
		auto output = allocator.GetColorGradeOutput();
		auto dstSize = output->GetSize();

		if (!mFramebuffer || mFramebuffer->GetSize() != dstSize)
		{
			FFramebufferTexture color{};
			color.ExternalTexture = output;

			FramebufferSpecification spec{};
			spec.DebugName = "ColorGrading";
			spec.Size = dstSize;
			spec.Attachments.AddColorAttachment(color);
			mFramebuffer = Framebuffer::Create(spec);
		}
	}
} // namespace BHive