#include "RotateComponent.h"
#include "scene/Entity.h"

namespace BHive
{
    void RotateComponent::OnUpdate(float dt)
    {
        auto entity = this->GetOwner();

        auto transform = entity->GetLocalTransform();
        transform.add_rotation({0, mRotateSpeed * dt, 0});

        entity->SetLocalTransform(transform);
    }

    void RotateComponent::Serialize(StreamWriter &ar) const
    {
        Component::Serialize(ar);
        ar(mRotateSpeed);
    }

    void RotateComponent::Deserialize(StreamReader &ar)
    {
        Component::Deserialize(ar);
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