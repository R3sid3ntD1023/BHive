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

        writer(mTransform);
    }

    void SceneComponent::Deserialize(StreamReader &reader)
    {
        ActorComponent::Deserialize(reader);

        reader(mTransform);
    }

    REFLECT(SceneComponent)
    {
        BEGIN_REFLECT(SceneComponent)
        (META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS();
    }

}