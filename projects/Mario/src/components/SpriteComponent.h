#pragma once

#include "Component.h"
#include "sprite/Sprite.h"

namespace BHive
{
	REFLECT_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct SpriteComponent : public Component
	{
		REFLECT_CONSTRUCTOR()
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent &other) = default;

		REFLECT_PROPERTY()
		glm::vec2 Tiling{1, 1};

		REFLECT_PROPERTY()
		glm::vec2 Size{1, 1};

		REFLECT_PROPERTY()
		FColor Color{0xffffffff};

		REFLECT_PROPERTY()
		Ref<Sprite> SpriteAsset;

		void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)
	};

} // namespace BHive