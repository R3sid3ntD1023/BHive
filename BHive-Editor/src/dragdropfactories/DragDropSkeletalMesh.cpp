#include "DragDropSkeletalMesh.h"
#include "world/GameObject.h"
#include "mesh/SkeletalMesh.h"
#include "world/components/SkeletalMeshComponent.h"

namespace BHive
{
	bool DragDropSkeletalMesh::CanCreateEntityFrom(const rttr::type &type)
	{
		return type.is_derived_from<SkeletalMesh>();
	}

	void DragDropSkeletalMesh::PostCreateEntity(const Ref<Asset> &asset, Ref<GameObject> &object)
	{
		if (auto mesh = Cast<SkeletalMesh>(asset))
			object->AddComponent<SkeletalMeshComponent>()->SkeletalMeshAsset = mesh;
	}

	REFLECT(DragDropSkeletalMesh)
	{
		BEGIN_REFLECT(DragDropSkeletalMesh)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive