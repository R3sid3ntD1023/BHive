#pragma once

#include "core/EnumAsByte.h"
#include "gfx/Color.h"
#include "renderers/QuadRenderer.h"
#include "ShapeComponent.h"
#include "sprite/FlipBook.h"

namespace BHive
{
	class FlipBook;

	struct FlipbookComponent : public ShapeComponent
	{
		bool mAutoPlay = true;

		TEnumAsByte<QuadRendererFlags_> mFlags = QuadRendererFlags_None;

		glm::vec2 mSize{1.0f};

		Color mColor{0xFFFFFFFF};

		TAssetHandle<FlipBook> mFlipBook;

		virtual AABB GetBoundingBox() const;

		virtual void OnRender(SceneRenderer *renderer) override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(ShapeComponent)
	};

	REFLECT_EXTERN(FlipbookComponent)

} // namespace BHive
