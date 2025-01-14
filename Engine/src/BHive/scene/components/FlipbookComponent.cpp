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

    void FlipbookComponent::Serialize(StreamWriter& ar) const
    {
        ShapeComponent::Serialize(ar);

        ar(mAutoPlay, mColor, mFlipBook, mSize, mFlags);
    }

    void FlipbookComponent::Deserialize(StreamReader& ar)
    {
        ShapeComponent::Deserialize(ar);

        ar(mAutoPlay, mColor, mFlipBook, mSize, mFlags);
    }

    REFLECT(FlipbookComponent)
    {
        BEGIN_REFLECT(FlipbookComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS()
                REFLECT_PROPERTY("Auto Play", mAutoPlay)
                    REFLECT_PROPERTY("Flags", mFlags)
                        REFLECT_PROPERTY("Size", mSize)
                            REFLECT_PROPERTY("Color", mColor)
                                REFLECT_PROPERTY("Flipbook", mFlipBook);
    }
}