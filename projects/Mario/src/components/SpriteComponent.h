#pragma once

#include "Component.h"
#include "assets/ASprite.h"

namespace BHive
{
	struct SpriteComponent : public Component
	{
		glm::vec2 Tiling{1, 1};

		glm::vec2 SpriteSize{1, 1};

		FColor SpriteColor{0xffffffff};

		TAssetHandle<ASprite> Sprite;

		void Update(float) override;
		void Render() override;

		REFLECTABLEV(Component)
	};

	REFLECT(SpriteComponent)
	{
		BEGIN_REFLECT(SpriteComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Tiling)
			REFLECT_PROPERTY(SpriteSize) REFLECT_PROPERTY(SpriteColor) REFLECT_PROPERTY(Sprite)
				REFLECT_METHOD(ADD_COMPONENT_FUNCTION_NAME, &GameObject::AddComponent<SpriteComponent>);
	}

} // namespace BHive