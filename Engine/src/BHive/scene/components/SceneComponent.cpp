#include "SceneComponent.h"
#include "scene/Entity.h"
#include "scene/World.h"

namespace BHive
{
    void SceneComponent::SetWorldTransform(const FTransform& transform)
    {
        mWorldTransform = transform;

        mLocalTransform = GetOwner()->GetWorldTransform().inverse() * mWorldTransform;
    }

    const FTransform& SceneComponent::GetWorldTransform() const
    {
        return mWorldTransform;
    }

    void SceneComponent::SetLocalTransform(const FTransform& transform)
    {
        mLocalTransform = transform;
        mWorldTransform = GetOwner()->GetWorldTransform() * mLocalTransform;
    }

    const FTransform& SceneComponent::GetLocalTransform() const
    {
        return mLocalTransform;
    }

    void SceneComponent::UpdateWorldTransform()
    {
        mWorldTransform = GetOwner()->GetWorldTransform() * mLocalTransform;
    }


    void SceneComponent::Serialize(StreamWriter &ar) const
    {
        Component::Serialize(ar);
        ar(mLocalTransform, mWorldTransform);
    }

    void SceneComponent::Deserialize(StreamReader &ar)
    {
        Component::Deserialize(ar);
        ar(mLocalTransform, mWorldTransform);
    }

    REFLECT(SceneComponent)
    {
        BEGIN_REFLECT(SceneComponent)
        (META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS();
    }

}