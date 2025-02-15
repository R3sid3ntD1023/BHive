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
#include <glad/glad.h>

CullingSystem::CullingSystem()
{
	mCullingShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "Shaders/CullingShader.glsl");

	const char *Varyings[1];
	Varyings[0] = "Position";
	glTransformFeedbackVaryings(mCullingShader->GetRendererID(), 1, Varyings, GL_INTERLEAVED_ATTRIBS);
}

void CullingSystem::Update(Universe *universe, float dt)
{
	mCullingShader->Bind();

	{
		auto view = universe->GetRegistry().view<IDComponent, MeshComponent>();
		for (auto &e : view)
		{
			auto [idcomponent, meshcomponent] = view.get(e);
			auto body = universe->GetBody(idcomponent.mID);
			auto world_transform = body->GetTransform();
			auto mesh = BHive::AssetManager::GetAsset<BHive::StaticMesh>(meshcomponent.mMeshHandle);

			mCullingShader->SetUniform<uint32_t>("uIsCulled", 1);
			// BHive::Renderer::SubmitTransform(world_transform);
			if (First)
			{
				BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mesh->GetVertexArray());
			}

			else
			{
				glDrawTransformFeedback(GL_TRIANGLES, mTransformFeedBack[]);
			}
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
				auto count = astroidcomponent.mNumInstances;

				Ref<BHive::VertexBuffer> instanced;
				if (!mInstanced.contains(handle))
				{
					auto buffer = BHive::VertexBuffer::Create(sizeof(glm::mat4) * count);
					buffer->SetLayout(BHive::BufferLayout{BHive::EShaderDataType::Mat4});
					mesh->GetVertexArray()->AddVertexBuffer(buffer);
					mInstanced.emplace(handle, buffer);
				}

				mCullingShader->SetUniform<uint32_t>("uIsCulled", 1);
				BHive::Renderer::SubmitTransform(world_transform);
				BHive::RenderCommand::DrawElementsInstanced(BHive::EDrawMode::Triangles, *mesh->GetVertexArray(), count);
			}
		}
		}

	First = false;
}