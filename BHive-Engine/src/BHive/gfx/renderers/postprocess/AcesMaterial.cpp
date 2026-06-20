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
		pass.CommandList.Push("Aces", this, &AcesMaterial::OnExecutePass, input);

		return mOutput;
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

	void AcesMaterial::OnExecutePass(IRendererContext &ctx, const Ref<Texture> &input)
	{
		const auto size = input->GetSize();
		glm::vec3 dispatch = {size.x, size.y, 1};

		Renderer::Get().ExecuteComputePass(PipelineRegistry::Get("ACES"), dispatch, this, &AcesMaterial::ExecutePass, input, mOutput);
	}

} // namespace BHive