#include "Component.h"

namespace BHive
{
	void Component::SetOwner(GameObject *owner)
	{
		mOwningObject = owner;
	}
	void Component::SetTickEnabled(bool enabled)
	{
		mTickEnabled = enabled;
	}

	void Component::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mTickEnabled);
	}

	void Component::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mTickEnabled);
	}
} // namespace BHive
