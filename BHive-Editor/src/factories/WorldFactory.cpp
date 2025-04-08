#include "WorldFactory.h"
#include "world/World.h"

namespace BHive
{
	Ref<Asset> WorldFactory::CreateNew()
	{
		auto asset = CreateRef<World>();
		OnAssetCreated.invoke(asset);
		return asset;
	}

	REFLECT_FACTORY(WorldFactory)
} // namespace BHive
