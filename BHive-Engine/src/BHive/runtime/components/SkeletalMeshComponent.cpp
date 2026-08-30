#include "SkeletalMeshComponent.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "runtime/GameObject.h"

namespace BHive
{

	void SkeletalMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mOverrideMaterials);
	}

	void SkeletalMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mOverrideMaterials);
	}

	void SkeletalMeshComponent::SetSkeletalMesh(MeshPtr mesh)
	{
		SkeletalMeshAsset = mesh;
		if (mesh && mesh.Is<SkeletalMesh>())
			mOverrideMaterials = mesh.As<SkeletalMesh>()->GetMaterialTable();
	}

	REFLECT(SkeletalMeshComponent)
	{
		BEGIN_REFLECT(SkeletalMeshComponent)
		(META_DATA(EClassMetaData::ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Skeletal Mesh", GetSkeletalMesh, SetSkeletalMesh) COMPONENT_IMPL();
	}
} // namespace BHive