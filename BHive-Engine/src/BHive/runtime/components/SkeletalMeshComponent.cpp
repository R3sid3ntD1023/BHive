#include "SkeletalMeshComponent.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "runtime/GameObject.h"

namespace BHive
{

	void SkeletalMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mOverrideMaterials, TAssetHandle(SkeletalMeshAsset));
	}

	void SkeletalMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mOverrideMaterials, TAssetHandle(SkeletalMeshAsset));
	}

	void SkeletalMeshComponent::SetSkeletalMesh(const Ref<SkeletalMesh> &mesh)
	{
		SkeletalMeshAsset = mesh;
		if (mesh)
			mOverrideMaterials = mesh->GetMaterialTable();
	}

	REFLECT(SkeletalMeshComponent)
	{
		BEGIN_REFLECT(SkeletalMeshComponent)
		(META_DATA(EClassMetaData::ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Skeletal Mesh", GetSkeletalMesh, SetSkeletalMesh) COMPONENT_IMPL();
	}
} // namespace BHive