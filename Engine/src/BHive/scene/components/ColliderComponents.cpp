#include "ColliderComponents.h"
#include "scene/SceneRenderer.h"
#include "scene/Actor.h"

namespace BHive
{
    void ColliderComponent::Serialize(StreamWriter& ar) const
    {
        ShapeComponent::Serialize(ar);

        ar(mOffset, mIsTrigger, mColor, mPhysicsMaterial);
    }

    void ColliderComponent::Deserialize(StreamReader& ar)
    {
        ShapeComponent::Deserialize(ar);

        ar(mOffset, mIsTrigger, mColor, mPhysicsMaterial);
    }

    void BoxComponent::OnRender(SceneRenderer *renderer)
    {
        LineRenderer::DrawBox(mExtents, mOffset, mColor, GetWorldTransform());
    }

    void BoxComponent::Serialize(StreamWriter& ar) const
    {
        ColliderComponent::Serialize(ar);
        ar(mExtents);
    }

    void BoxComponent::Deserialize(StreamReader& ar)
    {
        ColliderComponent::Deserialize(ar);
        ar(mExtents);
    }

    void SphereComponent::OnRender(SceneRenderer *renderer)
    {
        LineRenderer::DrawSphere(mRadius, 32, mOffset, mColor, GetWorldTransform());
    }

    void SphereComponent::Serialize(StreamWriter& ar) const
    {
        ColliderComponent::Serialize(ar);
        ar(mRadius);
    }

    void SphereComponent::Deserialize(StreamReader& ar)
    {
        ColliderComponent::Deserialize(ar);
        ar(mRadius);
    }

    REFLECT(ColliderComponent)
    {
        BEGIN_REFLECT(ColliderComponent)
        REFLECT_PROPERTY("Offset", mOffset)
        REFLECT_PROPERTY("Color", mColor)
        REFLECT_PROPERTY("IsTrigger", mIsTrigger)
        REFLECT_PROPERTY("PhysicsMaterial", mPhysicsMaterial);
    }

    REFLECT(BoxComponent)
    {
        BEGIN_REFLECT(BoxComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true))
        REQUIRED_COMPONENT_FUNCS()
        REFLECT_PROPERTY("Extents", mExtents);
    }

    REFLECT(SphereComponent)
    {
        BEGIN_REFLECT(SphereComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true))
        REQUIRED_COMPONENT_FUNCS()
        REFLECT_PROPERTY("Radius", mRadius);
    }
  
} // namespace  BHive
