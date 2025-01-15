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

	void Component::Save(cereal::BinaryOutputArchive &ar) const
	{
		ObjectBase::Save(ar);
		ar(mTickEnabled);
	}

	void Component::Load(cereal::BinaryInputArchive &ar)
	{
		ObjectBase::Load(ar);
		ar(mTickEnabled);
	}
} // namespace BHive