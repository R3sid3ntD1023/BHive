#include "RotateComponent.h"
#include "scene/Actor.h"

namespace BHive
{
    void RotateComponent::OnUpdate(float dt)
    {
        auto actor = this->GetOwningActor();

        auto transform = actor->GetLocalTransform();
        transform.add_rotation({0, mRotateSpeed * dt, 0});

        actor->SetLocalTransform(transform);
    }

    void RotateComponent::Serialize(StreamWriter &ar) const
    {
        ActorComponent::Serialize(ar);
        ar(mRotateSpeed);
    }

    void RotateComponent::Deserialize(StreamReader &ar)
    {
        ActorComponent::Deserialize(ar);
        ar(mRotateSpeed);
    }

    REFLECT(RotateComponent)
    {
        BEGIN_REFLECT(RotateComponent)
        (META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS()
                REFLECT_PROPERTY("RotateSpeed", mRotateSpeed);
    }
}