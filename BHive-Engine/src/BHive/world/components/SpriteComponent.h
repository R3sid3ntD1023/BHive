#pragma once

#include "World/Component.h"
#include "sprite/Sprite.h"

namespace BHive
{

	struct BHIVE_API SpriteComponent : public Component
	{
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent &other) = default;

		glm::vec2 Tiling{1, 1};

		glm::vec2 Size{1, 1};

		FColor Color{0xffffffff};

		Ref<Sprite> SpriteAsset;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)
	};

	REFLECT_EXTERN(SpriteComponent)
} // namespace BHive