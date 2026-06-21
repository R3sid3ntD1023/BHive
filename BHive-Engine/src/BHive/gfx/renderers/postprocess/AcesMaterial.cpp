#include "AcesMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	Ref<Texture> AcesMaterial::AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input)
	{
		auto output = allocator.GetAcesOuput();
		auto &pass = graph.AddPass("Aces", EPassType::OffScreen);
		pass.CommandList.Push("Aces", this, &AcesMaterial::OnExecutePass, input, output);

		return output;
	}

	void AcesMaterial::ExecutePass(FComputeBindings &b, const Ref<Texture> &in, const Ref<Texture> &out)
	{
		FImageInfo inputInfo{};
		inputInfo.Texture = in;
		inputInfo.Access = EImageAccess::READ;

		FImageInfo outputInfo{};
		outputInfo.Texture = out;
		outputInfo.Access = EImageAccess::WRITE;

		b.SampledImage("uSceneColor", inputInfo);
		b.StorageImage("uOutput", outputInfo);
	}

	void AcesMaterial::OnExecutePass(IRendererContext &ctx, const Ref<Texture> &in, const Ref<Texture>& out)
	{
		const auto dstSize = in->GetSize();
		glm::uvec3 dispatch = {(dstSize.x + 15u) / 16u, (dstSize.y + 15u) / 16u, 1};

		Renderer::Get().ExecuteComputePass(PipelineRegistry::Get("ACES"), dispatch, this, &AcesMaterial::ExecutePass, in, out);
	}

} // namespace BHive