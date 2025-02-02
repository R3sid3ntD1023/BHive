#include "EntityFactoryPrefab.h"
#include "prefab/Prefab.h"
#include "asset/EditorAssetManager.h"
#include "scene/World.h"

namespace BHive
{
	bool EntityFactoryPrefab::CanCreateEntityFrom(const FAssetMetaData &metaData)
	{
		return metaData.Type.is_derived_from<Prefab>();
	}

	void EntityFactoryPrefab::PostCreateEntity(const Ref<Asset> &asset, Ref<Entity> &entity)
	{
		auto prefab = Cast<Prefab>(asset);
		auto instance_world = prefab->GetInstance();
		auto world = entity->GetWorld();

		for (auto &[id, prefab_entity] : instance_world->GetEntities())
		{
			auto new_entity = world->DuplicateEntity(&*prefab_entity);
			new_entity->AttachTo(&*entity);
		}
	}

	REFLECT(EntityFactoryPrefab)
	{
		BEGIN_REFLECT(EntityFactoryPrefab)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive