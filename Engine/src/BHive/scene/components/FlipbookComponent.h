#pragma once

#include "ShapeComponent.h"
#include "gfx/Color.h"
#include "renderers/QuadRenderer.h"
#include "sprite/FlipBook.h"
#include "core/EnumAsByte.h"

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

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ShapeComponent)
	};

	REFLECT_EXTERN(FlipbookComponent)

}
