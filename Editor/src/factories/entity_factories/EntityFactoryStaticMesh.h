#pragma once

#include "EntityFactory.h"

namespace BHive
{
	class EntityFactoryStaticMesh : public EntityFactory
	{
	public:
		virtual bool CanCreateEntityFrom(const FAssetMetaData &metaData) override;

		virtual void PostCreateEntity(const Ref<Asset> &asset, Ref<Entity> &entity) override;

		REFLECTABLEV(EntityFactory)
	};

	REFLECT_EXTERN(EntityFactoryStaticMesh)
} // namespace BHive