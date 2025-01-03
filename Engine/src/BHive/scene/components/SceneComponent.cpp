#include "SceneComponent.h"
#include "scene/Actor.h"
#include "scene/World.h"

namespace BHive
{
    void SceneComponent::SetLocalTransform(const FTransform &transform)
    {
        mTransform = transform;
    }


    FTransform SceneComponent::GetWorldTransform() const
    {

        return GetOwningActor()->GetWorldTransform() * mTransform;
    }

    void SceneComponent::Serialize(StreamWriter &writer) const
    {
        ActorComponent::Serialize(writer);

        writer(mTransform, mPostLoadData.mParentID, mPostLoadData.mChildrenIDs);
    }

    void SceneComponent::Deserialize(StreamReader &reader)
    {
        ActorComponent::Deserialize(reader);

        reader(mTransform, mPostLoadData.mParentID, mPostLoadData.mChildrenIDs);
    }

    REFLECT(SceneComponent)
    {
        BEGIN_REFLECT(SceneComponent)
        (META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS();
    }

}