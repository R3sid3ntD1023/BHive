#include "Component.h"
#include "GameObject.h"

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

	FTransform Component::GetWorldTransform() const
	{
		if (mOwningObject)
			return mOwningObject->GetWorldTransform();

		return FTransform();
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
