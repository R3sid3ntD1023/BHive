#include "StaticMeshComponent.h"
#include "Renderers/MeshRenderer.h"
#include "Renderers/LineRenderer.h"
#include "GameObject.h"

namespace BHive
{
	void StaticMeshComponent::Render()
	{
		if (mStaticMeshAsset)
		{
			auto world_transform = GetOwner()->GetWorldTransform();
			MeshRenderer::DrawMesh(mStaticMeshAsset, mOverrideMaterials, world_transform);
			LineRenderer::DrawAABB(mStaticMeshAsset->GetBoundingBox(), Colors::Red, world_transform);
		}
	}

	void StaticMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mOverrideMaterials, TAssetHandle(mStaticMeshAsset));
	}

	void StaticMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(TAssetHandle(mStaticMeshAsset));
	}

	void StaticMeshComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ar(MAKE_NVP("OverrideMaterials", mOverrideMaterials), MAKE_NVP("StaticMesh", TAssetHandle(mStaticMeshAsset)));
	}

	void StaticMeshComponent::Load(cereal::JSONInputArchive &ar)
	{
		ar(MAKE_NVP("OverrideMaterials", mOverrideMaterials), MAKE_NVP("StaticMesh", TAssetHandle(mStaticMeshAsset)));
	}

	void StaticMeshComponent::SetStaticMesh(const Ref<StaticMesh> &mesh)
	{
		mStaticMeshAsset = mesh;
		mOverrideMaterials = mesh->GetMaterialTable();
	}

	REFLECT(StaticMeshComponent)
	{
		BEGIN_REFLECT(StaticMeshComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Material Overrides", mOverrideMaterials)
			REFLECT_PROPERTY("StaticMesh", GetStaticMesh, SetStaticMesh) COMPONENT_IMPL();
	}

} // namespace BHive
