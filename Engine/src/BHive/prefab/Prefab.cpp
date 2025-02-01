#include "Prefab.h"
#include "scene/World.h"

namespace BHive
{
	Ref<Entity> Prefab::CreateInstance(World *world)
	{
		auto instance = mInstance->Copy();
		world->AddEntity(instance);
		return instance;
	}

	void Prefab::SetEntityClass(const TSubClassOf<Entity> &entityClass)
	{
		mEntityClass = entityClass;
		if (mEntityClass)
		{
			mInstance = mEntityClass.get().create().get_value<Ref<Entity>>();
		}
	}

	void Prefab::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mEntityClass);
		if (mInstance)
			mInstance->Save(ar);
	}

	void Prefab::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);

		ar(mEntityClass);

		SetEntityClass(mEntityClass);
		if (mInstance)
			mInstance->Load(ar);
	}

	REFLECT(Prefab)
	{
		BEGIN_REFLECT(Prefab)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Entity", mInstance)
		REFLECT_PROPERTY("EntityClass", GetEntityClass, SetEntityClass);
	}

} // namespace BHive