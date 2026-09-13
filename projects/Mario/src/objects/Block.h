#pragma once

#include "Core.h"
#include "core/SubClassOf.h"
#include "world/GameObject.h"

namespace BHive
{
	class Sprite;

	struct MARIO_API BlockBase : public GameObject
	{
		BlockBase(const entt::entity &handle, World *world);

		REFLECTABLEV(GameObject)
	};

	struct MARIO_API Block : public BlockBase
	{
		Block(const entt::entity &handle, World *world);

		REFLECTABLEV(BlockBase)
	};

	struct MARIO_API QuestionBlock : public BlockBase
	{
		QuestionBlock(const entt::entity &handle, World *world);

		TSubClassOf<Block> SpawnedBlock;

		void OnCollisionEnter(struct ColliderComponent *component, GameObject *other);

		REFLECTABLEV(BlockBase)
	};

} // namespace BHive