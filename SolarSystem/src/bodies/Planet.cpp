#include "Planet.h"
#include "Universe.h"
#include <asset/AssetManager.h>
#include <gfx/RenderCommand.h>
#include <gfx/Shader.h>
#include <importers/TextureImporter.h>
#include <mesh/MeshImporter.h>
#include <mesh/primitives/Sphere.h>
#include <renderers/Renderer.h>

Planet::Planet(const entt::entity &entity, Universe *universe)
	: CelestrialBody(entity, universe)
{
}

void Planet::OnUpdate(const Ref<BHive::Shader> &shader, float dt)
{
	CelestrialBody::OnUpdate(shader, dt);

	auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(mData.mMeshHandle);
	if (!mesh)
		return;

	float seconds = mData.mRotationTime.ToSeconds();
	if (seconds != 0.f)
	{
		float theta = 360.f / seconds;
		mTransform.add_rotation({0, theta * dt * 1000.f, 0});
	}

	auto texture = BHive::AssetManager::GetAsset<BHive::Texture>(mData.mTextureHandle);
	if (!texture)
	{
		texture = BHive::Renderer::GetWhiteTexture();
	}

	texture->Bind();

	shader->SetUniform("uFlags", (uint32_t)mData.mFlags);
	shader->SetUniform("uColor", glm::vec3(1.0f));
	shader->SetUniform("uEmission", glm::vec3(0.0f));

	auto world_transform = GetTransform();
	BHive::Renderer::SubmitTransform(world_transform);
	BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mesh->GetVertexArray());
}

void Planet::Save(cereal::JSONOutputArchive &ar) const
{
	CelestrialBody::Save(ar);
	ar(MAKE_NVP("Data", mData));
}

void Planet::Load(cereal::JSONInputArchive &ar)
{
	CelestrialBody::Load(ar);
	ar(MAKE_NVP("Data", mData));
}

REFLECT(Planet)
{
	BEGIN_REFLECT(Planet)
	REFLECT_CONSTRUCTOR(const entt::entity &, Universe *);
}