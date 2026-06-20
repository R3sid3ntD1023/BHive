#include "ColorGradingMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/ShaderManager.h"

namespace BHive
{
	ColorGradingMaterial::ColorGradingMaterial()
	{
		Pipeline::ComputePipelineState state{};
		state.ShaderProgram = ShaderManager::Get("ColorGrading.glsl");
		PipelineRegistry::Register("COLOR_GRADING", state);
	}

	Ref<Texture> ColorGradingMaterial::AddToGraph(RenderGraph &graph, const Ref<Texture> &input)
	{
		auto pipeline = PipelineRegistry::Get("COLOR_GRADING");

		auto &pass = graph.AddPass("Bloom", EPassType::OffScreen);

		auto params = Params;

		pass.CommandList.Push(
			"Prefilter Scene Color",
			[input, output = mOutputTex, pipeline, params](IRendererContext &ctx)
			{
				auto dstSize = output->GetSize();
				glm::uvec3 dispatch = {dstSize, 1};

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

		return mOutputTex;
	}

	void ColorGradingMaterial::CreateResizableObjects(const glm::uvec2 &size)
	{
		FTextureCreateInfo info{};
		info.Format = EFormat::RGBA8;
		info.Roles |= ETextureRole::ComputeWrite;
		info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		info.DebugName = "ColorGradingTex";

		mOutputTex = Texture2D::Create(size, info);
	}
} // namespace BHive