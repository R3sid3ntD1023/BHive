#include "Planet.h"
#include <gfx/RenderCommand.h>
#include <gfx/Shader.h>
#include <importers/TextureImporter.h>
#include <mesh/MeshImporter.h>
#include <mesh/primitives/Sphere.h>
#include <renderers/Renderer.h>

Planet::Planet()
{
	if (!mSphere)
	{
		mSphere = CreateRef<BHive::PSphere>(1.f, 64, 64);
	}
}

void Planet::OnUpdate(const Ref<BHive::Shader> &shader, float dt)
{
	CelestrialBody::OnUpdate(shader, dt);

	float theta = 360.f / mData.mRotationTime.ToSeconds();

	mTransform.add_rotation({0, theta * dt * 1000.f, 0});

	auto texture = mTexture;
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
	BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mSphere->GetVertexArray());
}

void Planet::Initialize(const PlanetData &data)
{
	if (!data.mTexturePath.empty())
	{
		mTexture = BHive::TextureImporter::Import(RESOURCE_PATH + data.mTexturePath);
	}

	mData = data;
}

void Planet::Save(cereal::JSONOutputArchive &ar) const
{
	CelestrialBody::Save(ar);
	ar(mData);
}

void Planet::Load(cereal::JSONInputArchive &ar)
{
	CelestrialBody::Load(ar);
	ar(mData);

	Initialize(mData);
}