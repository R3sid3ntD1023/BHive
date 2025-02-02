#pragma once

#include "asset/Factory.h"
#include "asset/AssetMetaData.h"
#include "math/Transform.h"

namespace BHive
{
	class World;
	class Entity;

	class EntityFactory
	{
	public:
		virtual Ref<Entity> CreateEntityFrom(const FAssetMetaData &metaData, World *world, const FTransform &transform);

		virtual bool CanCreateEntityFrom(const FAssetMetaData &metaData) { return false; }

		virtual void PostCreateEntity(const Ref<Asset> &asset, Ref<Entity> &entity) {};

		REFLECTABLEV()
	};

	Ref<EntityFactory> GetEntityFactory(const FAssetMetaData &metaData);
} // namespace BHive