#pragma once

#include "Block.h"

namespace BHive
{
	struct Item : public Block
	{
		Item(const entt::entity &handle, World *world);

		REFLECTABLEV(Block)

	private:
		void OnCollisionTriggerEnter(struct ColliderComponent *component, GameObject *other);
	};
} // namespace BHive