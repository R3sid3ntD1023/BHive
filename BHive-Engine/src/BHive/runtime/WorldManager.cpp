#include "WorldManager.h"
#include "World.h"
#include "core/platform/Platform.h"

namespace BHive
{
	void WorldManager::CreateWorld()
	{
		mCurrentWorld = CreateRef<World>();
	}

	void WorldManager::OpenWorld(const Ref<World> &world)
	{
		mCurrentWorld = world;
	}

} // namespace BHive