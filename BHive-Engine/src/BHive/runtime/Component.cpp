#include "Component.h"
#include "GameObject.h"

namespace BHive
{
	void Component::SetOwner(GameObject *owner)
	{
		mOwningObject = owner;
	}

	FTransform Component::GetWorldTransform() const
	{
		if (mOwningObject)
			return mOwningObject->GetWorldTransform();

		return FTransform();
	}

	void Component::Save(cereal::BinaryOutputArchive &ar) const
	{
	}

	void Component::Load(cereal::BinaryInputArchive &ar)
	{
	}
} // namespace BHive
