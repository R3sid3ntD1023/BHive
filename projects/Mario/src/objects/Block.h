#pragma once

#include "GameObject.h"

namespace BHive
{
	class Sprite;

	REFLECT_CLASS(BHive::ClassMetaData_Spawnable)
	struct BlockBase : public GameObject
	{
		REFLECT_CONSTRUCTOR()
		BlockBase(const entt::entity &handle, World *world);

		REFLECTABLE_CLASS(GameObject)
	};

	REFLECT_CLASS(BHive::ClassMetaData_Spawnable)
	struct Block : public BlockBase
	{
		REFLECT_CONSTRUCTOR()
		Block(const entt::entity &handle, World *world);

		REFLECTABLE_CLASS(BlockBase)
	};

	REFLECT_CLASS(BHive::ClassMetaData_Spawnable)
	struct QuestionBlock : public BlockBase
	{
		REFLECT_CONSTRUCTOR()
		QuestionBlock(const entt::entity &handle, World *world);

		REFLECTABLE_CLASS(BlockBase)
	};

} // namespace BHive