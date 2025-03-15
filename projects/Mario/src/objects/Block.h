#pragma once

#include "GameObject.h"

namespace BHive
{
	class Sprite;

	struct Block : public GameObject
	{
		Block(const entt::entity &handle, World *world);

		void SetSize(const glm::vec2 &size);

		void SetTiling(const glm::vec2 &tiling);
		void SetSprite(const Ref<Sprite> &sprite);
	};
} // namespace BHive