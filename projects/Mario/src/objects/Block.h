#pragma once

#include "GameObject.h"

namespace BHive
{
	class Sprite;

	struct BlockBase : public GameObject
	{
		BlockBase(const entt::entity &handle, World *world);
	};

	struct Block : public BlockBase
	{
		Block(const entt::entity &handle, World *world);

		void SetSize(const glm::vec2 &size);

		void SetTiling(const glm::vec2 &tiling);
		void SetSprite(const Ref<Sprite> &sprite);

		REFLECTABLEV(GameObject)
	};

	struct QuestionBlock : public BlockBase
	{
		QuestionBlock(const entt::entity &handle, World *world);

		REFLECTABLEV(GameObject)
	};
} // namespace BHive