#pragma once

#include "GameObject.h"

namespace BHive
{
	class ASprite;

	struct BlockBase : public GameObject
	{
		BlockBase(World *world);
	};

	struct Block : public BlockBase
	{
		Block(World *world);

		void SetSize(const glm::vec2 &size);

		void SetTiling(const glm::vec2 &tiling);
		void SetSprite(const Ref<ASprite> &sprite);
	};

	struct QuestionBlock : public BlockBase
	{
		QuestionBlock(World *world);
	};
} // namespace BHive