#include "EntityFactorySkeletalMesh.h"
#include "scene/Entity.h"
#include "scene/components/SkeletalMeshComponent.h"

namespace BHive
{
	bool EntityFactorySkeletalMesh::CanCreateEntityFrom(const FAssetMetaData &metaData)
	{
		return metaData.Type.is_derived_from<SkeletalMesh>();
	}

	void EntityFactorySkeletalMesh::PostCreateEntity(const Ref<Asset> &asset, Ref<Entity> &entity)
	{
		entity->AddNewComponent<SkeletalMeshComponent>(asset->GetName())->SetSkeletalMesh(Cast<SkeletalMesh>(asset));
	}

	REFLECT(EntityFactorySkeletalMesh)
	{
		BEGIN_REFLECT(EntityFactorySkeletalMesh)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive