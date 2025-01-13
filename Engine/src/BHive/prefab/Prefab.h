#pragma once

#include "asset/Asset.h"
#include "scene/Entity.h"

namespace BHive
{
	class World;

	class Prefab : public Asset
	{
	public:
		Prefab();

		Ref<Entity> CreateInstance(World *world);

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		Ref<Entity> mInstance;
	};

	REFLECT(Prefab)
	{
		BEGIN_REFLECT(Prefab)
		REFLECT_PROPERTY("Entity", mInstance);
	}

} // namespace BHive