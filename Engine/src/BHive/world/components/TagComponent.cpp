#include "GameObject.h"
#include "TagComponent.h"

namespace BHive
{
	TagComponent::TagComponent()
	{
		SetTickEnabled(false);
	}

	void TagComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mID, Name, Group);
	}

	void TagComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mID, Name, Group);
	}

	void TagComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ar(MAKE_NVP("ID", mID), MAKE_NVP(Name), MAKE_NVP(Group));
	}

	void TagComponent::Load(cereal::JSONInputArchive &ar)
	{
		ar(MAKE_NVP("ID", mID), MAKE_NVP(Name), MAKE_NVP(Group));
	}

	REFLECT(TagComponent)
	{
		BEGIN_REFLECT(TagComponent)
		REFLECT_PROPERTY_READ_ONLY("ID", mID) REFLECT_PROPERTY(Name) REFLECT_PROPERTY(Group) COMPONENT_IMPL();
	}
} // namespace BHive