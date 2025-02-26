#include "EntityFactoryStaticMesh.h"
#include "scene/components/StaticMeshComponent.h"
#include "scene/Entity.h"
#include "mesh/StaticMesh.h"

namespace BHive
{
	bool EntityFactoryStaticMesh::CanCreateEntityFrom(const FAssetMetaData &metaData)
	{
		return metaData.Type.is_derived_from<StaticMesh>();
	}

	void EntityFactoryStaticMesh::PostCreateEntity(const Ref<Asset> &asset, Ref<Entity> &entity)
	{
		entity->AddNewComponent<StaticMeshComponent>(asset->GetName())->SetStaticMesh(Cast<StaticMesh>(asset));
	}

	REFLECT(EntityFactoryStaticMesh)
	{
		BEGIN_REFLECT(EntityFactoryStaticMesh)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive