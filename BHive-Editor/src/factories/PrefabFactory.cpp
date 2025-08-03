#include "PrefabFactory.h"
#include "prefab/Prefab.h"

namespace BHive
{
	Ref<Asset> PrefabFactory::CreateNew()
	{
		auto prefab = CreateRef<Prefab>();
		OnAssetCreated.invoke(prefab);
		return prefab;
	}

	REFLECT_FACTORY(PrefabFactory)
} // namespace BHive