#include "Star.h"
#include <gfx/RenderCommand.h>
#include <gfx/Shader.h>
#include <mesh/primitives/Sphere.h>
#include <renderers/Renderer.h>

namespace SolarSystem
{
	Star::Star()
	{
		if (!mSphere)
		{
			mSphere = CreateRef<BHive::PSphere>(1.f, 64, 64);
		}
	}

	void Star::Update(const Ref<BHive::Shader> &shader, float)
	{

		BHive::Renderer::GetWhiteTexture()->Bind();
		shader->SetUniform("uColor", (glm::vec3)Color);
		shader->SetUniform("uEmission", (glm::vec3)Emission);

		BHive::PointLight light{};
		light.mColor = Color;
		light.mRadius = Radius;
		light.mBrightness = Brightness;

		BHive::Renderer::SubmitTransform(mTransform);
		BHive::Renderer::SubmitPointLight(mTransform.get_translation(), light);
		BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mSphere->GetVertexArray());
	}
	void Star::Save(cereal::JSONOutputArchive &ar) const
	{
		CelestrialBody::Save(ar);
		ar(Brightness, Radius, Color, Emission);
	}

	void Star::Load(cereal::JSONInputArchive &ar)
	{
		CelestrialBody::Load(ar);
		ar(Brightness, Radius, Color, Emission);
	}
} // namespace SolarSystem