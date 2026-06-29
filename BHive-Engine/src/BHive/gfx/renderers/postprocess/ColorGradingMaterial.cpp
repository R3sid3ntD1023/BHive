#include "ColorGradingMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	Ref<Texture> ColorGradingMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		mInput = input;
		mOutput = allocator.GetColorGradeOutput();

		auto &pass = graph.AddPass("Color Grading Pass", EPassType::OffScreen);
		pass.BeginPhase();

		pass.Push(mInput, EImageAccess::ComputeSampled);
		pass.Push(mOutput, EImageAccess::ComputeStorageWrite);
		pass.Push("Compute Color Grade", this, &ColorGradingMaterial::DoColorGrading);

		pass.EndPhase();

		pass.BeginPhase();
		pass.Push(mOutput, EImageAccess::ColorRead);
		pass.EndPhase();

		return mOutput;
	}
	void ColorGradingMaterial::DoColorGrading(IRendererContext &ctx)
	{

		auto dstSize = mOutput->GetSize();

		glm::uvec3 dispatch = {(dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1};

		// prefilter
		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get("COLOR_GRADING"), dispatch,
			[this](FComputeBindings &b)
			{

				b.Bind("uTonemapped", mInput);
				b.Bind("uOutput", mOutput);
				b.Set("uLift", Params.Lift);
				b.Set("uGamma", Params.Gamma);
				b.Set("uGain", Params.Gain);
				b.Set("uSaturation", Params.Saturation);
			});
	}
} // namespace BHive