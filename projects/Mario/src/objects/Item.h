#pragma once

#include "Block.h"

namespace BHive
{
	DECLARE_CLASS(BHive::ClassMetaData_Spawnable)
	struct Item : public Block
	{
		DECLARE_CONSTRUCTOR()
		Item(const entt::entity &handle, World *world);

		REFLECTABLE_CLASS(Block)

	private:
		void OnCollisionTriggerEnter(struct ColliderComponent *component, GameObject *other);
	};
} // namespace BHive