#include "Component.h"
#include "scene/Entity.h"

namespace BHive
{
    World *Component::GetWorld() const
    {
        if (GetOwner())
            return GetOwner()->GetWorld();
        return nullptr;
    }

    void Component::SetTickEnabled(bool enabled)
    {
        mTickEnabled = enabled;
    }

    void Component::Serialize(StreamWriter &ar) const
    {
        ObjectBase::Serialize(ar);
        ar(mTickEnabled);
    }

    void Component::Deserialize(StreamReader &ar)
    {
        ObjectBase::Deserialize(ar);
        ar(mTickEnabled);
    }
}