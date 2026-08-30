#include "StaticMeshComponent.h"
#include "runtime/GameObject.h"
#include "gfx/mesh/StaticMesh.h"

namespace BHive
{

	void StaticMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mOverrideMaterials, mStaticMeshAsset);
	}

	void StaticMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mOverrideMaterials, mStaticMeshAsset);
	}

	void StaticMeshComponent::SetStaticMesh(MeshPtr mesh)
	{
		mStaticMeshAsset = mesh;
		if (mesh && mesh.Is<StaticMesh>())
			mOverrideMaterials = mesh.As<StaticMesh>()->GetMaterialTable();
	}

	REFLECT(StaticMeshComponent)
	{
		BEGIN_REFLECT(StaticMeshComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Material Overrides", mOverrideMaterials)
			REFLECT_PROPERTY("StaticMesh", GetStaticMesh, SetStaticMesh) COMPONENT_IMPL();
	}

} // namespace BHive
