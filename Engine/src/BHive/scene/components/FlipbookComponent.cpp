#include "FlipbookComponent.h"
#include "scene/SceneRenderer.h"
#include "scene/Entity.h"

namespace BHive
{
	AABB FlipbookComponent::GetBoundingBox() const
	{
		// return bounds of current sprite
		return AABB();
	}

	void FlipbookComponent::OnRender(SceneRenderer *renderer)
	{
		if (mFlipBook)
		{
			auto sprite = mFlipBook->GetCurrentSprite();
			QuadRenderer::DrawSprite(mSize, mColor, GetWorldTransform(), sprite, mFlags);
		}
	}

	void FlipbookComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ShapeComponent::Save(ar);

		ar(MAKE_NVP("AutoPlay", mAutoPlay), MAKE_NVP("Color", mColor),
		   MAKE_NVP("FlipBook", mFlipBook), MAKE_NVP("Size", mSize), MAKE_NVP("Flags", mFlags));
	}

	void FlipbookComponent::Load(cereal::JSONInputArchive &ar)
	{
		ShapeComponent::Load(ar);

		ar(MAKE_NVP("AutoPlay", mAutoPlay), MAKE_NVP("Color", mColor),
		   MAKE_NVP("FlipBook", mFlipBook), MAKE_NVP("Size", mSize), MAKE_NVP("Flags", mFlags));
	}

	REFLECT(FlipbookComponent)
	{
		BEGIN_REFLECT(FlipbookComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Auto Play", mAutoPlay) REFLECT_PROPERTY("Flags", mFlags)
				REFLECT_PROPERTY("Size", mSize) REFLECT_PROPERTY("Color", mColor)
					REFLECT_PROPERTY("Flipbook", mFlipBook);
	}
} // namespace BHive