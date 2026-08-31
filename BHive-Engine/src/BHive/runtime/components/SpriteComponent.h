#pragma once

#include "runtime/Component.h"
#include "gfx/registries/Handles.h"
#include "gfx/Color.h"

namespace BHive
{

	struct BHIVE_API SpriteComponent : public Component
	{
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent &other) = default;

		glm::vec2 Tiling{1, 1};

		glm::vec2 Size{1, 1};

		FColor Color = FColor::White;

		SpritePtr Sprite;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)
	};

	REFLECT_EXTERN(SpriteComponent)
} // namespace BHive