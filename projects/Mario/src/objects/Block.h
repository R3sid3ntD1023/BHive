#pragma once

#include "GameObject.h"
#include "core/SubClassOf.h"

namespace BHive
{
	class Sprite;

	DECLARE_CLASS(BHive::ClassMetaData_Spawnable)
	struct BlockBase : public GameObject
	{
		DECLARE_CONSTRUCTOR()
		BlockBase(const entt::entity &handle, World *world);

		REFLECTABLE_CLASS(GameObject)
	};

	DECLARE_CLASS(BHive::ClassMetaData_Spawnable)
	struct Block : public BlockBase
	{
		DECLARE_CONSTRUCTOR()
		Block(const entt::entity &handle, World *world);

		REFLECTABLE_CLASS(BlockBase)
	};

	DECLARE_CLASS(BHive::ClassMetaData_Spawnable)
	struct QuestionBlock : public BlockBase
	{
		DECLARE_CONSTRUCTOR()
		QuestionBlock(const entt::entity &handle, World *world);

		DECLARE_PROPERTY()
		TSubClassOf<Block> SpawnedBlock;

		void OnCollisionEnter(struct ColliderComponent *component, GameObject *other);

		REFLECTABLE_CLASS(BlockBase)
	};

} // namespace BHive