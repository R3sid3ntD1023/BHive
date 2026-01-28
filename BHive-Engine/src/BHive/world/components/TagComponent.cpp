#include "World/GameObject.h"
#include "TagComponent.h"

namespace BHive
{
	void TagComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mID, Name, Group);
	}

	void TagComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mID, Name, Group);
	}

	REFLECT(TagComponent)
	{
		BEGIN_REFLECT(TagComponent)
		REFLECT_PROPERTY_READ_ONLY("ID", mID) REFLECT_PROPERTY(Name) REFLECT_PROPERTY(Group) COMPONENT_IMPL();
	}
} // namespace BHive