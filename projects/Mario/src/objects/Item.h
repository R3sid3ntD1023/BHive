#pragma once

#include "Block.h"
#include "Core.h"

namespace BHive
{
	struct MARIO_API Item : public Block
	{
		Item(const entt::entity &handle, World *world);

		REFLECTABLEV(Block)

	private:
		void OnCollisionTriggerEnter(struct ColliderComponent *component, GameObject *other);
	};
} // namespace BHive