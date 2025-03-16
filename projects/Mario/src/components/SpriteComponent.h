#pragma once

#include "Component.h"
#include "sprite/Sprite.h"

namespace BHive
{
	struct SpriteComponent : public Component
	{
		glm::vec2 Tiling{1, 1};

		glm::vec2 SpriteSize{1, 1};

		FColor SpriteColor{0xffffffff};

		Ref<Sprite> Sprite;

		UUID SpriteHandle = UUID::Null;

		void Update(float) override;
	};

} // namespace BHive