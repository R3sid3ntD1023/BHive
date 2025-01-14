#include "SceneComponent.h"
#include "scene/Entity.h"
#include "scene/World.h"

namespace BHive
{
    void SceneComponent::SetWorldTransform(const FTransform& transform)
    {
        mLocalTransform = GetOwner()->GetWorldTransform().inverse() * transform;
    }

    FTransform SceneComponent::GetWorldTransform() const
    {
        return  GetOwner()->GetWorldTransform() * mLocalTransform;
    }

    void SceneComponent::SetLocalTransform(const FTransform& transform)
    {
        mLocalTransform = transform;
    }

    const FTransform& SceneComponent::GetLocalTransform() const
    {
        return mLocalTransform;
    }


    void SceneComponent::Serialize(StreamWriter &ar) const
    {
        Component::Serialize(ar);
        ar(mLocalTransform);
    }

    void SceneComponent::Deserialize(StreamReader &ar)
    {
        Component::Deserialize(ar);
        ar(mLocalTransform);
    }

    REFLECT(SceneComponent)
    {
        BEGIN_REFLECT(SceneComponent)
        (META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS();
    }

}