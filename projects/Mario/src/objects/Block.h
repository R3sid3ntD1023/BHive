#pragma once

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

		REFLECTABLEV(BlockBase)
	};

	REFLECT_EXTERN(BlockBase)
	REFLECT_EXTERN(Block)
	REFLECT_EXTERN(QuestionBlock)
} // namespace BHive