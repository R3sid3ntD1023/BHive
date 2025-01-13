#pragma once

#include "scene/components/ShapeComponent.h"
#include "gfx/Color.h"
#include "sprite/Sprite.h"
#include "renderers/QuadRenderer.h"
#include "core/EnumAsByte.h"

namespace BHive
{
	class Sprite;

	struct BHIVE SpriteComponent : public ShapeComponent
	{
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent &) = default;

		Color mColor = 0xFFFFFFFF;

		TAssetHandle<Sprite> mSprite;

		glm::vec2 mSize{1.0f};

		TEnumAsByte<QuadRendererFlags_> mFlags = QuadRendererFlags_None;

		virtual AABB GetBoundingBox() const
		{
			return AABB(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{mSize, 1.0f});
		}

		virtual void OnRender(SceneRenderer *renderer) override;

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(ShapeComponent)
	};

	REFLECT_EXTERN(SpriteComponent)

} // namespace BHive
