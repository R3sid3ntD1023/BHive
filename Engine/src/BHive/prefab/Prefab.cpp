#include "Prefab.h"
#include "world/GameObject.h"
#include "world/World.h"

namespace BHive
{
	Prefab::Prefab()
	{
		mInstance = CreateRef<World>();
	}

	void Prefab::CreateInstance(World *world)
	{
		for (auto &[id, object] : mInstance->GetGameObjects())
		{
			auto duplicated = object->Duplicate();
			world->AddGameObject(duplicated);
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