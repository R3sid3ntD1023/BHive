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

    void Component::Save(cereal::JSONOutputArchive& ar) const
    {
		ObjectBase::Save(ar);
		ar(MAKE_NVP("IsTickEnabled", mTickEnabled));
    }

    void Component::Load(cereal::JSONInputArchive& ar)
    {
		ObjectBase::Load(ar);
		ar(MAKE_NVP("IsTickEnabled", mTickEnabled));
    }
}