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
#include "core/profiler/CPUGPUProfiler.h"
#include <gfx/StorageBuffer.h>
#include <gfx/BindlessTexture.h>
#include <glad/glad.h>

RenderSystem::RenderSystem()
{
	mShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "Shaders/Planet.glsl");
	mZPrePassShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "Shaders/ZPrePass.glsl");
}

void RenderSystem::Update(Universe *universe, float dt)
{
	CPU_PROFILER_FUNCTION();

	if (mPreZPass)
	{
		BHive::RenderCommand::DepthFunc(GL_LESS);
		BHive::RenderCommand::ColorMask(0, 0, 0, 0);

		ZPrePass(universe);

		BHive::RenderCommand::DepthFunc(GL_EQUAL);
		BHive::RenderCommand::ColorMask(1, 1, 1, 1);

		Render(universe);

		BHive::RenderCommand::DepthFunc(GL_LESS);
	}
	else
	{
		BHive::RenderCommand::DepthFunc(GL_LESS);
		BHive::RenderCommand::ColorMask(1, 1, 1, 1);

		Render(universe);
	}
}

void RenderSystem::SetPreZPass(bool enabled)
{
	mPreZPass = enabled;
}

void RenderSystem::ZPrePass(class Universe *universe)
{

	// BHive::RenderCommand::EnableDepthMask(true);

	mZPrePassShader->Bind();

	{

		GPU_PROFILER_SCOPED("DrawInstancedMeshes::ZPass");

		auto view = universe->GetRegistry().view<IDComponent, AstroidComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, astroidcomponent] = view.get(e);
			auto body = universe->GetBody(idcomponent.mID);
			auto world_transform = body->GetTransform();

			mZPrePassShader->SetUniform("uInstanced", true);
			const auto amount = astroidcomponent.mNumInstances;
			for (auto &mesh_id : astroidcomponent.mMeshHandles)
			{
				auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(mesh_id);

				if (!mesh)
					continue;

				if (!mStorageBuffer.contains(mesh_id))
				{
					auto buffer = BHive::StorageBuffer::Create(1, sizeof(glm::mat4) * amount);
					mStorageBuffer.emplace(mesh_id, buffer);
				}

				mStorageBuffer[mesh_id]->SetData(astroidcomponent.GetMatrices().data(), sizeof(glm::mat4) * amount);

				BHive::Renderer::SubmitTransform(world_transform);

				BHive::RenderCommand::DrawElementsInstanced(BHive::EDrawMode::Triangles, *mesh->GetVertexArray(), amount);
			}
		}
	}

	{

		GPU_PROFILER_SCOPED("DrawMeshes::ZPass");

		auto view = universe->GetRegistry().view<IDComponent, MeshComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, meshcomponent] = view.get(e);
			auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(meshcomponent.mMeshHandle);

			if (mesh)
			{
				auto body = universe->GetBody(idcomponent.mID);
				auto world_transform = body->GetTransform();

				mZPrePassShader->SetUniform("uInstanced", false);
				BHive::Renderer::SubmitTransform(world_transform);
				BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mesh->GetVertexArray());
			}
		}
	}
}

void RenderSystem::Render(class Universe *universe)
{
	mShader->Bind();

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

		GPU_PROFILER_SCOPED("DrawInstancedMeshes");

		auto view = universe->GetRegistry().view<IDComponent, AstroidComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, astroidcomponent] = view.get(e);
			auto body = universe->GetBody(idcomponent.mID);
			auto world_transform = body->GetTransform();

			auto texture = BHive::AssetManager::GetAsset<BHive::Texture2D>(astroidcomponent.mTextureHandle);
			texture->Bind();
			// if (!mTextures.contains(astroidcomponent.mTextureHandle))
			// {
			// 	mTextures[astroidcomponent.mTextureHandle] = BHive::BindlessTexture::Create(texture);
			// }

			// mShader->SetBindlessTexture("uTexture", mTextures[astroidcomponent.mTextureHandle]->GetHandle());
			// texture->Bind();
			mShader->SetUniform("uInstanced", true);
			mShader->SetUniform("uFlags", (uint32_t)astroidcomponent.mFlags);
			mShader->SetUniform("uColor", astroidcomponent.mColor);
			mShader->SetUniform("uEmission", astroidcomponent.mEmission);

			const auto amount = astroidcomponent.mNumInstances;
			for (auto &mesh_id : astroidcomponent.mMeshHandles)
			{
				auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(mesh_id);

				if (!mesh)
					continue;

				if (!mStorageBuffer.contains(mesh_id))
				{
					auto buffer = BHive::StorageBuffer::Create(1, sizeof(glm::mat4) * amount);
					mStorageBuffer.emplace(mesh_id, buffer);
				}

				mStorageBuffer[mesh_id]->SetData(astroidcomponent.GetMatrices().data(), sizeof(glm::mat4) * amount);

				BHive::Renderer::SubmitTransform(world_transform);

				BHive::RenderCommand::DrawElementsInstanced(BHive::EDrawMode::Triangles, *mesh->GetVertexArray(), amount);
			}
		}
	}

	{

		GPU_PROFILER_SCOPED("DrawMeshes");

		auto view = universe->GetRegistry().view<IDComponent, MeshComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, meshcomponent] = view.get(e);
			auto texture = BHive::AssetManager::GetAsset<BHive::Texture2D>(meshcomponent.mTextureHandle);
			auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(meshcomponent.mMeshHandle);

			if (mesh && texture)
			{
				auto body = universe->GetBody(idcomponent.mID);
				auto world_transform = body->GetTransform();

				// if (!mTextures.contains(meshcomponent.mTextureHandle))
				// {
				// 	mTextures[meshcomponent.mTextureHandle] = BHive::BindlessTexture::Create(texture);
				// }

				texture->Bind();
				// mShader->SetBindlessTexture("uTexture", mTextures[meshcomponent.mTextureHandle]->GetHandle());
				mShader->SetUniform("uInstanced", false);
				mShader->SetUniform("uFlags", (uint32_t)meshcomponent.mFlags);
				mShader->SetUniform("uColor", meshcomponent.mColor);
				mShader->SetUniform("uEmission", meshcomponent.mEmission);
				BHive::Renderer::SubmitTransform(world_transform);

				BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mesh->GetVertexArray());
			}
		}
	}
}
