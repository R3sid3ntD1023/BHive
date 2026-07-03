#include "ColorGradingMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	Ref<Texture> ColorGradingMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto output = allocator.GetColorGradeOutput();

		auto bindings = FComputeBindings(PipelineRegistry::Get("COLOR_GRADING"))
							.Set({"uTonemapped", input})
							.Set({"uOutput", output})
							.Set<glm::vec3>({"uLift", Params.Lift})
							.Set<glm::vec3>({"uGamma", Params.Gamma})
							.Set<glm::vec3>({"uGain", Params.Gain})
							.Set<float>({"uSaturation", Params.Saturation});

		auto dstSize = output->GetSize();

		auto &pass = graph.AddPass("Color Grading Pass", EPassType::OffScreen);
		pass.BeginPhase();
		pass.Push(input, EImageAccess::ComputeSampled);
		pass.Push(output, EImageAccess::ComputeStorageWrite);
		pass.Emplace<CmdBindMaterial>()(&bindings);
		pass.Emplace<CmdDisptach>()((dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1);
		pass.EndPhase();

		pass.BeginPhase();
		pass.Push(output, EImageAccess::ColorRead);
		pass.EndPhase();

		return output;
	}
} // namespace BHive