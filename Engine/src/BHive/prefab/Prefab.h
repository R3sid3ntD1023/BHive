#pragma once

#include "asset/Asset.h"
#include "scene/Entity.h"
#include "core/SubClassOf.h"

namespace BHive
{
	class World;

	class Prefab : public Asset
	{
	public:
		Prefab() = default;

		Ref<Entity> CreateInstance(World *world);

		void SetEntityClass(const TSubClassOf<Entity> &entityClass);

		const TSubClassOf<Entity> &GetEntityClass() const { return mEntityClass; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		Ref<Entity> mInstance;
		TSubClassOf<Entity> mEntityClass;
	};

} // namespace BHive