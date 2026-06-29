#include "AcesMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	Ref<Texture> AcesMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		mInput = input;
		mOutput = allocator.GetAcesOutput();

		auto &pass = graph.AddPass("Aces", EPassType::OffScreen);

		pass.BeginPhase();

		pass.Push(mInput, EImageAccess::ComputeSampled);
		pass.Push(mOutput, EImageAccess::ComputeStorageWrite);
		pass.Push("Aces", this, &AcesMaterial::OnExecutePass);

		pass.EndPhase();

		pass.BeginPhase();
		pass.Push(mOutput, EImageAccess::ColorRead);
		pass.EndPhase();

		return mOutput;
	}

	void AcesMaterial::ExecutePass(FComputeBindings &b)
	{
		b.Bind("uSceneColor", mInput);
		b.Bind("uOutput", mOutput);
	}

	void AcesMaterial::OnExecutePass(IRendererContext &ctx)
	{
		const auto dstSize = mOutput->GetSize();
		glm::uvec3 dispatch = {(dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1};

		Renderer::Get().ExecuteComputePass(PipelineRegistry::Get("ACES"), dispatch, this, &AcesMaterial::ExecutePass);
	}

} // namespace BHive