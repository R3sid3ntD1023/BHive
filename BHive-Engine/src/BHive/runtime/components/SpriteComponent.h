#pragma once

#include "asset/AssetHandle.h"
#include "gfx/Color.h"
#include "runtime/Component.h"

namespace BHive
{
	class Sprite;

	struct BHIVE_API SpriteComponent : public Component
	{
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent &other) = default;

		glm::vec2 Tiling{1, 1};

		glm::vec2 Size{1, 1};

		FColor Color = FColor::White;

		AssetHandle<Sprite> Sprite;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)
	};

	REFLECT_EXTERN(SpriteComponent)
} // namespace BHive