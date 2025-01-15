#include "FlipbookComponent.h"
#include "scene/SceneRenderer.h"

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

	void FlipbookComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ShapeComponent::Save(ar);

		ar(mAutoPlay, mColor, mFlipBook, mSize, mFlags);
	}

	void FlipbookComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ShapeComponent::Load(ar);

		ar(mAutoPlay, mColor, mFlipBook, mSize, mFlags);
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