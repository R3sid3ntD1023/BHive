#include "Prefab.h"

namespace BHive
{
	Prefab::Prefab()
	{
		mInstance = CreateRef<Entity>();
	}

	Ref<Entity> Prefab::CreateInstance(World *world)
	{
		return mInstance->Copy();
	}

	void Prefab::Save(cereal::JSONOutputArchive &ar) const
	{
		Asset::Save(ar);
		mInstance->Save(ar);
	}

	void Prefab::Load(cereal::JSONInputArchive &ar)
	{
		Asset::Load(ar);
		mInstance->Load(ar);
	}
} // namespace BHive