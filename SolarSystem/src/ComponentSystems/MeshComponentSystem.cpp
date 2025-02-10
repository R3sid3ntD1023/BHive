#include "MeshComponentSystem.h"

#include <asset/AssetManager.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/RenderCommand.h>
#include <mesh/StaticMesh.h>
#include <renderers/Renderer.h>

#include "components/MeshComponent.h"
#include "components/IDComponent.h"
#include "Universe.h"
#include "CelestrialBody.h"

MeshComponentSystem::MeshComponentSystem()
{
	mShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "/Shaders/Planet.glsl");
}

void MeshComponentSystem::Update(Universe *universe, float dt)
{
	mShader->Bind();

	auto view = universe->GetRegistry().view<IDComponent, MeshComponent>();
	for (auto &e : view)
	{
		auto [idcomponent, meshcomponent] = view.get(e);
		auto texture = BHive::AssetManager::GetAsset<BHive::Texture2D>(meshcomponent.mTextureHandle);
		auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(meshcomponent.mMeshHandle);

		if (mesh)
		{
			auto body = universe->GetBody(idcomponent.mID);
			auto world_transform = body->GetTransform();

			texture->Bind();
			mShader->SetUniform("uFlags", (uint32_t)meshcomponent.mFlags);
			mShader->SetUniform("uColor", meshcomponent.mColor);
			mShader->SetUniform("uEmission", meshcomponent.mEmission);
			BHive::Renderer::SubmitTransform(world_transform);
			BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mesh->GetVertexArray());
		}
	}
}