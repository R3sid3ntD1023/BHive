#include "RenderSystem.h"

#include <asset/AssetManager.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/RenderCommand.h>
#include <mesh/StaticMesh.h>
#include <renderers/Renderer.h>

#include "components/MeshComponent.h"
#include "components/IDComponent.h"
#include "components/StarComponent.h"
#include "components/AstroidComponent.h"
#include "Universe.h"
#include "CelestrialBody.h"

RenderSystem::RenderSystem()
{
	mShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "/Shaders/Planet.glsl");
	mInstanceShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "/Shaders/PlanetInstanced.glsl");
}

void RenderSystem::Update(Universe *universe, float dt)
{
	{
		auto view = universe->GetRegistry().view<IDComponent, StarComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, starcomponent] = view.get(e);

			auto world_transform = universe->GetBody(idcomponent.mID)->GetTransform();

			BHive::PointLight light;
			light.mBrightness = starcomponent.mBrightness;
			light.mRadius = starcomponent.mRadius;
			light.mColor = starcomponent.mColor;
			BHive::Renderer::SubmitPointLight(world_transform.get_translation(), light);
		}
	}

	{
		mInstanceShader->Bind();

		auto view = universe->GetRegistry().view<IDComponent, AstroidComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, astroidcomponent] = view.get(e);
			auto body = universe->GetBody(idcomponent.mID);
			auto world_transform = body->GetTransform();

			auto texture = BHive::AssetManager::GetAsset<BHive::Texture2D>(astroidcomponent.mTextureHandle);

			mInstanceShader->SetBindlessTexture("u_Texture", texture->GetResourceHandle());
			// texture->Bind();
			mInstanceShader->SetUniform("uFlags", (uint32_t)astroidcomponent.mFlags);
			mInstanceShader->SetUniform("uColor", astroidcomponent.mColor);
			mInstanceShader->SetUniform("uEmission", astroidcomponent.mEmission);

			const auto amount = astroidcomponent.mNumInstances;
			for (auto &mesh_id : astroidcomponent.mMeshHandles)
			{
				auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(mesh_id);

				Ref<BHive::VertexBuffer> instanced;
				if (!mVertexBuffers.contains(mesh_id))
				{
					auto buffer = BHive::VertexBuffer::Create(sizeof(glm::mat4) * amount);
					buffer->SetLayout(BHive::BufferLayout{BHive::EShaderDataType::Mat4});
					mesh->GetVertexArray()->AddVertexBuffer(buffer);
					mVertexBuffers.emplace(mesh_id, buffer);
				}

				instanced = mVertexBuffers.at(mesh_id);
				instanced->SetData(astroidcomponent.GetMatrices().data(), sizeof(glm::mat4) * amount);

				BHive::Renderer::SubmitTransform(world_transform);
				BHive::RenderCommand::DrawElementsInstanced(BHive::EDrawMode::Triangles, *mesh->GetVertexArray(), amount);
			}
		}
	}

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

				// texture->Bind();
				mShader->SetUniform("uFlags", (uint32_t)meshcomponent.mFlags);
				mShader->SetUniform("uColor", meshcomponent.mColor);
				mShader->SetUniform("uEmission", meshcomponent.mEmission);
				mShader->SetBindlessTexture("u_Texture", texture->GetResourceHandle());
				BHive::Renderer::SubmitTransform(world_transform);
				BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mesh->GetVertexArray());
			}
		}
	}
}