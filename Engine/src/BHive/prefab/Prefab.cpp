#include "Prefab.h"
#include "scene/World.h"

namespace BHive
{
	Prefab::Prefab()
	{
		mInstance = CreateRef<World>();
	}

	void Prefab::CreateInstance(World *world)
	{
		for (auto &[id, entity] : mInstance->GetEntities())
		{
			auto duplicated = mInstance->DuplicateEntity(&*entity);
			world->AddEntity(duplicated);
		}
	}

	void Prefab::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		mInstance->Save(ar);
	}

	void Prefab::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		mInstance->Load(ar);
	}

	REFLECT(Prefab)
	{
		BEGIN_REFLECT(Prefab)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Instance", mInstance);
	}

} // namespace BHive