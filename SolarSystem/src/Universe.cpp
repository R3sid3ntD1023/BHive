#include "Universe.h"
#include "CelestrialBody.h"
#include <asset/AssetManager.h>

#include <renderers/Renderer.h>
#include <gfx/RenderCommand.h>
#include <gfx/Texture.h>
#include <mesh/StaticMesh.h>
#include <gfx/Shader.h>

#include "components/IDComponent.h"
#include "components/MeshComponent.h"
#include "components/StarComponent.h"
#include "components/PlanetComponent.h"

void Universe::AddBody(const Ref<CelestrialBody> &body)
{
	mBodies.emplace(BHive::UUID(), body);
}

void Universe::Update(const Ref<BHive::Shader> &shader, float dt)
{
	{
		auto view = mRegistry.view<IDComponent, PlanetComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, planetcomponent] = view.get(e);
			auto body = mBodies.at(idcomponent.mID);
			body->GetLocalTransform().add_rotation({0.f, planetcomponent.mTheta * dt * 1000.f, 0.f});
		}
	}

	{
		auto view = mRegistry.view<IDComponent, StarComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, starcomponent] = view.get(e);

			auto world_transform = mBodies.at(idcomponent.mID)->GetTransform();

			BHive::PointLight light;
			light.mBrightness = starcomponent.mBrightness;
			light.mRadius = starcomponent.mRadius;
			light.mColor = starcomponent.mColor;
			BHive::Renderer::SubmitPointLight(world_transform.get_translation(), light);
		}
	}

	{
		auto view = mRegistry.view<IDComponent, MeshComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, meshcomponent] = view.get(e);
			auto texture = BHive::AssetManager::GetAsset<BHive::Texture2D>(meshcomponent.mTextureHandle);
			auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(meshcomponent.mMeshHandle);

			if (mesh)
			{
				auto body = mBodies.at(idcomponent.mID);
				auto world_transform = body->GetTransform();

				texture->Bind();
				shader->SetUniform("uFlags", (uint32_t)meshcomponent.mFlags);
				shader->SetUniform("uColor", meshcomponent.mColor);
				shader->SetUniform("uEmission", meshcomponent.mEmission);
				BHive::Renderer::SubmitTransform(world_transform);
				BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mesh->GetVertexArray());
			}
		}
	}
}

void Universe::Save(cereal::JSONOutputArchive &ar) const
{
	ar(cereal::make_size_tag(mBodies.size()));
	for (auto &[id, body] : mBodies)
	{
		ar.startNode();

		ar(MAKE_NVP("ID", id));
		ar(MAKE_NVP("Type", body->get_type()));
		body->Save(ar);

		ar.finishNode();
	}
}

void Universe::Load(cereal::JSONInputArchive &ar)
{
	size_t size = 0;
	ar(cereal::make_size_tag(size));

	mBodies.reserve(size);

	for (size_t i = 0; i < size; i++)
	{
		ar.startNode();

		BHive::UUID id = 0;
		rttr::type type = BHive::InvalidType;

		ar(MAKE_NVP("ID", id));
		ar(MAKE_NVP("Type", type));

		if (type)
		{
			auto body = type.create({mRegistry.create(), this}).get_value<Ref<CelestrialBody>>();
			body->Load(ar);

			mBodies.emplace(id, body);
		}

		ar.finishNode();
	}
}

Ref<CelestrialBody> Universe::GetBody(const BHive::UUID &id) const
{
	return mBodies.at(id);
}
