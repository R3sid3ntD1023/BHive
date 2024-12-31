#include "ActorComponent.h"
#include "scene/Actor.h"

namespace BHive
{
    World *ActorComponent::GetWorld() const
    {
        if (GetOwningActor())
            return GetOwningActor()->GetWorld();
        return nullptr;
    }

    void ActorComponent::SetTickEnabled(bool enabled)
    {
        mTickEnabled = enabled;
    }

    void ActorComponent::Serialize(StreamWriter &writer) const
    {
        ObjectBase::Serialize(writer);
        writer(mTickEnabled);
    }

    void ActorComponent::Deserialize(StreamReader &reader)
    {
        ObjectBase::Deserialize(reader);
        reader(mTickEnabled);
    }
}