#include "ColorGradingMaterial.h"
#include "gfx/Framebuffer.h"
#include "gfx/Pipeline.h"
#include "gfx/factories/GFXFactories.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{
	ColorGradingMaterial::ColorGradingMaterial()
	{
		mMaterial = MaterialFactory::Create("ColorGrading.glsl");
	}

	TexturePtr ColorGradingMaterial::AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set)
	{
		auto input = set.PrevOutput;
		auto output = mFramebuffer.As<Framebuffer>()->GetColorAttachment();

		auto material = mMaterial.As<Material>();
		material->SetTexture("uTonemapped", FTextureBinding(input));
		material->SetParam("uLift", MaterialParam(Params.Lift));
		material->SetParam("uGamma", MaterialParam(Params.Gamma));
		material->SetParam("uGain", MaterialParam(Params.Gain));
		material->SetParam("uSaturation", MaterialParam(Params.Saturation));

		auto &pass = graph.AddPass("Color Grading Pass", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Graphics);
		pass.UseFramebuffer(mFramebuffer);
		pass.UseTexture(input, EImageUsage::ColorRead);
		pass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("DEFAULT"));
		pass.Emplace<CmdBindMaterial>()(material);
		pass.Emplace<CmdDrawFullScreen>()();
		pass.EndPhase();

		pass.BeginPhase(EPhaseType::Transfer);
		pass.UseTexture(output, EImageUsage::ColorRead);
		pass.EndPhase();

		return output;
	}

	void ColorGradingMaterial::Resize(const glm::uvec2 &size)
	{
		if (size.x <= 0 || size.y <= 0)
			return;

		mFramebuffer.As<Framebuffer>()->Resize(size);
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
		mFramebuffer = FramebufferFactory::Create(spec);
	}

	REFLECT(ColorGradingMaterial::FParams)
	{
		BEGIN_REFLECT(ColorGradingMaterial::FParams)
		REFLECT_PROPERTY(Lift)(META_DATA(EPropertyMetaData_Default, glm::vec3{-0.02f COMMA - 0.02f COMMA - 0.01f}))
			REFLECT_PROPERTY(Gamma)(META_DATA(EPropertyMetaData_Default, glm::vec3{0.95f COMMA 0.97f COMMA 1.00f}))
				REFLECT_PROPERTY(Gain)(META_DATA(EPropertyMetaData_Default, glm::vec3{1.05f COMMA 1.03f COMMA 1.00f})) REFLECT_PROPERTY(Saturation)(META_DATA(EPropertyMetaData_Default, 1.1f));
	}
} // namespace BHive