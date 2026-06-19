#include "AcesMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/ShaderManager.h"

namespace BHive
{
	AcesMaterial::AcesMaterial()
	{
		Pipeline::ComputePipelineState state{};
		state.ShaderProgram = ShaderManager::Get("Aces.glsl");
		PipelineRegistry::Register("ACES", state);
	}

	Ref<Texture> AcesMaterial::AddToGraph(RenderGraph &graph, const Ref<Texture> &input)
	{
		auto &pass = graph.AddPass("Aces", EPassType::OffScreen);

		const auto size = input->GetSize();
		glm::vec3 dispatch = {size.x ,size.y, 1};
		auto output = mOutput;

		pass.CommandList.Push(
			"Aces",
			[input, output, dispatch](IRendererContext &ctx)
			{
				Renderer::Get().ExecuteComputePass(
					PipelineRegistry::Get("ACES"), dispatch,
					[input, output](FComputeBindings &b)
					{
						FImageInfo inputInfo{};
						inputInfo.Texture = input;
						inputInfo.Access = EImageAccess::READ;

						FImageInfo outputInfo{};
						outputInfo.Texture = output;
						outputInfo.Access = EImageAccess::WRITE;

						b.SampledImage("uSceneColor", inputInfo);
						b.StorageImage("uOutput", outputInfo);
					});
		});

		return output;
	}

	void AcesMaterial::CreateResizableObjects(const glm::uvec2 &size)
	{
		FTextureCreateInfo info{};
		info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		info.Format = EFormat::RGBA8;
		info.Roles |= ETextureRole::ComputeWrite;
		info.DebugName = "AcesOutput";
		mOutput = Texture2D::Create(size, info);
	}

} // namespace BHive