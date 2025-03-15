#pragma once

#include "sprite/Sprite.h"
#include "Component.h"

namespace BHive
{
	struct SpriteComponent : public Component
	{
		glm::vec2 Tiling{1, 1};

		glm::vec2 SpriteSize{1, 1};

		FColor SpriteColor{0xffffffff};

		Ref<Sprite> Sprite;

		UUID SpriteHandle = UUID::Null;
	};

} // namespace BHive