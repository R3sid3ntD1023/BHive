#include "ColorGradingMaterial.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	ColorGradingMaterial::ColorGradingMaterial()
	{
		mMaterial = CreateScope<Material>("ColorGrading.glsl");
	}

	Ref<Texture> ColorGradingMaterial::AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set)
	{
		auto input = set.PrevOutput;
		auto output = mFramebuffer->GetColorAttachment();

		mMaterial->SetTexture("uTonemapped", FTextureBinding(input));
		mMaterial->SetParam("uLift", MaterialParam(Params.Lift));
		mMaterial->SetParam("uGamma", MaterialParam(Params.Gamma));
		mMaterial->SetParam("uGain", MaterialParam(Params.Gain));
		mMaterial->SetParam("uSaturation", MaterialParam(Params.Saturation));

		auto &pass = graph.AddPass("Color Grading Pass", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Graphics);
		pass.UseFramebuffer(mFramebuffer);
		pass.UseTexture(input, EImageUsage::ColorRead);
		pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
		pass.Emplace<CmdBindMaterial>()(mMaterial.get());
		pass.Emplace<CmdDrawFullScreen>()();
		pass.EndPhase();

		pass.BeginPhase(EPhaseType::Transfer);
		pass.UseTexture(output, EImageUsage::ColorRead);
		pass.EndPhase();

		return output;
	}

	void ColorGradingMaterial::Init(const glm::uvec2 &size)
	{
		FTextureCreateInfo info{};
		info.Format = EFormat::RGBA32F;
		info.Roles |= ETextureRole::RenderTarget;
		info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		info.DebugName = "ColorGradingTex";

		FFramebufferTexture color{info, ETextureType::TEXTURE_2D};

		FramebufferSpecification spec{};
		spec.DebugName = "ColorGrading";
		spec.Size = size;
		spec.Attachments.AddColorAttachment(color);
		mFramebuffer = Framebuffer::Create(spec);
	}

	REFLECT(ColorGradingMaterial::FParams)
	{
		BEGIN_REFLECT(ColorGradingMaterial::FParams)
		REFLECT_PROPERTY(Lift)(META_DATA(EPropertyMetaData_Default, glm::vec3{-0.02f COMMA - 0.02f COMMA - 0.01f}))
			REFLECT_PROPERTY(Gamma)(META_DATA(EPropertyMetaData_Default, glm::vec3{0.95f COMMA 0.97f COMMA 1.00f}))
				REFLECT_PROPERTY(Gain)(META_DATA(EPropertyMetaData_Default, glm::vec3{1.05f COMMA 1.03f COMMA 1.00f})) REFLECT_PROPERTY(Saturation)(META_DATA(EPropertyMetaData_Default, 1.1f));
	}
} // namespace BHive