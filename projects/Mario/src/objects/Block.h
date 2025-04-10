#pragma once

#include "core/SubClassOf.h"
#include "GameObject.h"

namespace BHive
{
	class Sprite;

	struct BlockBase : public GameObject
	{
		BlockBase(const entt::entity &handle, World *world);

		REFLECTABLEV(GameObject)
	};

	struct Block : public BlockBase
	{
		Block(const entt::entity &handle, World *world);

		REFLECTABLEV(BlockBase)
	};

	struct QuestionBlock : public BlockBase
	{
		QuestionBlock(const entt::entity &handle, World *world);

		TSubClassOf<Block> SpawnedBlock;

		void OnCollisionEnter(struct ColliderComponent *component, GameObject *other);

		REFLECTABLEV(BlockBase)
	};

} // namespace BHive