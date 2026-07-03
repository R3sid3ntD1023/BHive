#include "AcesMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	Ref<Texture> AcesMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto output = allocator.GetAcesOutput();

		auto bindings = FComputeBindings(PipelineRegistry::Get("ACES"))
			.Set({"uSceneColor", input})
			.Set({"uOutput", output});

		const auto dstSize = output->GetSize();

		auto &pass = graph.AddPass("Aces", EPassType::OffScreen);

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