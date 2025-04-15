#include "asset/AssetManager.h"
#include "CelestrialBody.h"
#include "gfx/Shader.h"
#include "gfx/textures/Texture2D.h"
#include "mesh/indirect_mesh/IndirectMesh.h"
#include "mesh/BaseMesh.h"
#include "MeshComponent.h"
#include "renderer/RenderPipeline.h"
#include "renderer/ShaderInstance.h"
#include "gfx/ShaderManager.h"

BEGIN_NAMESPACE(BHive)

MeshComponent::MeshComponent()
{
	auto shader = ShaderManager::Get().Get("Planet.glsl");
	mShaderInstance = CreateRef<ShaderInstance>(shader);
}

void MeshComponent::Update(float dt)
{
	if (auto pipeline = GetRenderPipelineManager().GetCurrentPipeline())
	{
		pipeline->SubmitMesh(mIndirectMesh, GetOwner()->GetWorldTransform(), mShaderInstance);
	}
}

void MeshComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP(Color), MAKE_NVP(Emission), MAKE_NVP(Flags), MAKE_NVP(Texture), MAKE_NVP(Mesh));
}

void MeshComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP(Color), MAKE_NVP(Emission), MAKE_NVP(Flags), MAKE_NVP(Texture), MAKE_NVP(Mesh));

	if (auto renderable = AssetManager::GetAsset<BaseMesh>(Mesh))
	{
		mIndirectMesh = CreateRef<IndirectRenderable>();
		InitIndirectMesh(renderable, mIndirectMesh);
	}

	mShaderInstance->SetParameter("uColor", Color);
	mShaderInstance->SetParameter("uEmission", Emission);
	mShaderInstance->SetParameter("uFlags", (uint32_t)Flags);

	if (auto texture = AssetManager::GetAsset<Texture2D>(Texture))
	{
		mShaderInstance->SetTexture("uTexture", texture);
	}
}

void MeshComponent::InitIndirectMesh(const Ref<BaseMesh> &renderable, Ref<IndirectRenderable> &indirect)
{
	indirect->Init(renderable);
}

REFLECT(MeshComponent)
{
	BEGIN_REFLECT(MeshComponent)
	REFLECT_CONSTRUCTOR() COMPONENT_IMPL();
}

END_NAMESPACE