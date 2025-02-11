#include "AstroidComponentSystem.h"
#include "Universe.h"
#include "components/AstroidComponent.h"
#include "CelestrialBody.h"
#include "components/IDComponent.h"
#include <asset/AssetManager.h>
#include <mesh/StaticMesh.h>
#include <gfx/RenderCommand.h>
#include <gfx/Shader.h>
#include <renderers/Renderer.h>

AstroidComponentSystem::AstroidComponentSystem()
{
	mInstanceShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "/Shaders/PlanetInstanced.glsl");
}

void AstroidComponentSystem::Update(Universe *universe, float dt)
{
	mInstanceShader->Bind();

	auto view = universe->GetRegistry().view<IDComponent, AstroidComponent>();
	for (auto &e : view)
	{
		auto [idcomponent, astroidcomponent] = view.get(e);
		auto body = universe->GetBody(idcomponent.mID);
		auto world_transform = body->GetTransform();

		auto texture = BHive::AssetManager::GetAsset<BHive::Texture2D>(astroidcomponent.mTextureHandle);
		texture->Bind();

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