#pragma once

#include "EntityFactory.h"

namespace BHive
{
	struct EntityFactoryPrefab : public EntityFactory
	{
		virtual bool CanCreateEntityFrom(const FAssetMetaData &metaData) override;

		virtual void PostCreateEntity(const Ref<Asset> &asset, Ref<Entity> &entity) override;

		REFLECTABLEV(EntityFactory)
	};

	REFLECT_EXTERN(EntityFactoryPrefab)
} // namespace BHive