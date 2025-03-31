#pragma once

#include "Component.h"
#include "sprite/Sprite.h"

namespace BHive
{
	DECLARE_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct SpriteComponent : public Component
	{
		DECLARE_CONSTRUCTOR()
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent &other) = default;

		DECLARE_PROPERTY()
		glm::vec2 Tiling{1, 1};

		DECLARE_PROPERTY()
		glm::vec2 Size{1, 1};

		DECLARE_PROPERTY()
		FColor Color{0xffffffff};

		DECLARE_PROPERTY()
		Ref<Sprite> SpriteAsset;

		void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)
	};

} // namespace BHive