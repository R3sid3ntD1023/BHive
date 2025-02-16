#include "CelestrialBody.h"
#include "components/AstroidComponent.h"
#include "components/IDComponent.h"
#include "components/MeshComponent.h"
#include "CullingSystem.h"
#include "Universe.h"
#include <asset/AssetManager.h>
#include <gfx/RenderCommand.h>
#include <renderers/Renderer.h>
#include <gfx/Shader.h>
#include <mesh/StaticMesh.h>
#include <gfx/StorageBuffer.h>
#include <glad/glad.h>
#include <core/profiler/CPUGPUProfiler.h>

CullingSystem::CullingSystem()
{
	mCullingShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "Shaders/CullingShader.glsl");
}

void CullingSystem::Update(Universe *universe, float dt)
{
	GPU_PROFILER_FUNCTION();

	mCullingShader->Bind();

	{
		auto view = universe->GetRegistry().view<IDComponent, MeshComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, meshcomponent] = view.get(e);
			auto body = universe->GetBody(idcomponent.mID);
			auto world_transform = body->GetTransform();
			auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(meshcomponent.mMeshHandle);

			mCullingShader->SetUniform<uint32_t>("uIsCulled", 0);
			mCullingShader->SetUniform("uInstanced", false);
			BHive::Renderer::SubmitTransform(world_transform);
			BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mesh->GetVertexArray());
		}
	}

	{
		auto view = universe->GetRegistry().view<IDComponent, AstroidComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, astroidcomponent] = view.get(e);
			auto body = universe->GetBody(idcomponent.mID);
			auto world_transform = body->GetTransform();

			for (auto &handle : astroidcomponent.mMeshHandles)
			{
				auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(handle);

				if (!mesh)
					continue;

				auto count = astroidcomponent.mNumInstances;

				if (!mInstanced.contains(handle))
				{
					auto buffer = BHive::StorageBuffer::Create(0, sizeof(glm::mat4) * count);
					mInstanced.emplace(handle, buffer);
				}

				mInstanced[handle]->SetData(astroidcomponent.GetMatrices().data(), sizeof(glm::mat4) * count);

				mCullingShader->SetUniform<uint32_t>("uIsCulled", 0);
				mCullingShader->SetUniform("uInstanced", true);
				BHive::Renderer::SubmitTransform(world_transform);
				BHive::RenderCommand::DrawElementsInstanced(BHive::EDrawMode::Triangles, *mesh->GetVertexArray(), count);
			}
		}
	}
}