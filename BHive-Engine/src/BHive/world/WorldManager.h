#pragma once

#include "core/Core.h"

namespace BHive
{
	class World;

	class WorldManager
	{
	public:
		void CreateWorld();

		void OpenWorld(const Ref<World> &world);

		const Ref<World> &GetCurrentWorld() const { return mCurrentWorld; }

	private:
		Ref<World> mCurrentWorld;
	};
} // namespace BHive