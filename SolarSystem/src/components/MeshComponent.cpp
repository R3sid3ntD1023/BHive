#include "asset/AssetManager.h"
#include "CelestrialBody.h"
#include "gfx/BindlessTexture.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "indirect_mesh/IndirectMesh.h"
#include "mesh/IRenderableAsset.h"
#include "MeshComponent.h"
#include "renderer/RenderPipeline.h"
#include "renderer/ShaderInstance.h"

MeshComponent::MeshComponent()
{
	auto shader = BHive::ShaderLibrary::Load(RESOURCE_PATH "Shaders/Planet.glsl");
	mShaderInstance = CreateRef<BHive::ShaderInstance>(shader);
}

void MeshComponent::Update(float dt)
{
	auto transform = GetOwner()->GetTransform();
	if (auto pipeline = BHive::GetRenderPipelineManager().GetCurrentPipeline())
	{
		pipeline->SubmitMesh(mIndirectMesh, GetOwner()->GetTransform(), mShaderInstance);
	}
}

void MeshComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP(Color), MAKE_NVP(Emission), MAKE_NVP(Flags), MAKE_NVP(Texture), MAKE_NVP(Mesh));
}

void MeshComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP(Color), MAKE_NVP(Emission), MAKE_NVP(Flags), MAKE_NVP(Texture), MAKE_NVP(Mesh));

	if (auto renderable = BHive::AssetManager::GetAsset<BHive::IRenderableAsset>(Mesh))
	{
		mIndirectMesh = CreateRef<BHive::IndirectRenderable>();
		InitIndirectMesh(renderable, mIndirectMesh);
	}

	if (auto texture = BHive::AssetManager::GetAsset<BHive::Texture2D>(Texture))
	{
		mBindlessTexture = BHive::BindlessTexture::Create(texture);
	}

	mShaderInstance->SetParameter("uColor", Color);
	mShaderInstance->SetParameter("uEmission", Emission);
	mShaderInstance->SetParameter("uFlags", (uint32_t)Flags);
	mShaderInstance->SetTexture("uTexture", *mBindlessTexture);
}

void MeshComponent::InitIndirectMesh(
	const Ref<BHive::IRenderableAsset> &renderable, Ref<BHive::IndirectRenderable> &indirect)
{
	indirect->Init(renderable);
}

REFLECT(MeshComponent)
{
	BEGIN_REFLECT(MeshComponent)
	REFLECT_CONSTRUCTOR();
}