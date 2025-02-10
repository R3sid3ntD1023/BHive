#include "Star.h"
#include "Universe.h"
#include <gfx/RenderCommand.h>
#include <gfx/Shader.h>
#include <mesh/primitives/Sphere.h>
#include <renderers/Renderer.h>

Star::Star(const entt::entity &entity, Universe *universe)
	: CelestrialBody(entity, universe)
{
}

void Star::OnUpdate(const Ref<BHive::Shader> &shader, float dt)
{
	CelestrialBody::OnUpdate(shader, dt);
	auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(1);
	if (!mesh)
		return;

	BHive::Renderer::GetWhiteTexture()->Bind();
	shader->SetUniform("uColor", (glm::vec3)Color);
	shader->SetUniform("uEmission", (glm::vec3)Emission);

	BHive::PointLight light{};
	light.mColor = Color;
	light.mRadius = Radius;
	light.mBrightness = Brightness;

	auto world_transform = GetTransform();
	BHive::Renderer::SubmitTransform(world_transform);
	BHive::Renderer::SubmitPointLight(world_transform.get_translation(), light);
	BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mesh->GetVertexArray());
}
void Star::Save(cereal::JSONOutputArchive &ar) const
{
	CelestrialBody::Save(ar);
	ar(MAKE_NVP("Brightness", Brightness), MAKE_NVP("Radius", Radius), MAKE_NVP("Color", Color), MAKE_NVP("Emission", Emission));
}

void Star::Load(cereal::JSONInputArchive &ar)
{
	CelestrialBody::Load(ar);
	ar(MAKE_NVP("Brightness", Brightness), MAKE_NVP("Radius", Radius), MAKE_NVP("Color", Color), MAKE_NVP("Emission", Emission));
}

REFLECT(Star)
{
	BEGIN_REFLECT(Star)
	REFLECT_CONSTRUCTOR(const entt::entity &, Universe *);
}
