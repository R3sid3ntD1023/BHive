#include "DragDropStaticMesh.h"
#include "world/components/StaticMeshComponent.h"
#include "world/gameObject.h"
#include "mesh/StaticMesh.h"

namespace BHive
{
	bool DragDropStaticMesh::CanCreateEntityFrom(const rttr::type &type)
	{
		return type.is_derived_from<StaticMesh>();
	}

	void DragDropStaticMesh::PostCreateEntity(const Ref<Asset> &asset, Ref<GameObject> &object)
	{
		if (auto mesh = Cast<StaticMesh>(asset))
			object->AddComponent<StaticMeshComponent>()->StaticMeshAsset = mesh;
	}

	REFLECT(DragDropStaticMesh)
	{
		BEGIN_REFLECT(DragDropStaticMesh)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive