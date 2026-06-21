#include "ColorGradingMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	Ref<Texture> ColorGradingMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto pipeline = PipelineRegistry::Get("COLOR_GRADING");

		auto &pass = graph.AddPass("Bloom", EPassType::OffScreen);

		auto params = Params;
		auto output = allocator.GetColorGradeOuput();

		pass.CommandList.Push(
			"Prefilter Scene Color",
			[input, output = output, pipeline, params](IRendererContext &ctx)
			{
				auto dstSize = output->GetSize();
				glm::uvec3 dispatch = {(dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1};

				// prefilter
				Renderer::Get().ExecuteComputePass(
					pipeline, dispatch,
					[input, output, params](FComputeBindings &b)
					{
						FImageInfo in{};
						in.Texture = input;
						in.Access = EImageAccess::READ;

						FImageInfo out{};
						out.Texture = output;
						out.Access = EImageAccess::WRITE;

						b.SampledImage("uTonemapped", in);
						b.StorageImage("uOutput", out);
						b.Set("uLift", params.Lift);
						b.Set("uGamma", params.Gamma);
						b.Set("uGain", params.Gain);
						b.Set("uSaturation", params.Saturation);
					});
			});

		return output;
	}
} // namespace BHive