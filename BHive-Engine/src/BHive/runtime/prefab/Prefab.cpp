#include "Prefab.h"
#include "runtime/GameObject.h"
#include "runtime/World.h"

namespace BHive
{
	Prefab::Prefab()
	{
		mInstance = CreateRef<World>();
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