#include "GameObject.h"
#include "RelationshipComponent.h"

namespace BHive
{
	RelationshipComponent::RelationshipComponent()
	{
		SetTickEnabled(false);
	}

	void RelationshipComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Parent, Children);
	}

	void RelationshipComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Parent, Children);
	}

	void RelationshipComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ar(MAKE_NVP(Parent), MAKE_NVP(Parent));
	}

	void RelationshipComponent::Load(cereal::JSONInputArchive &ar)
	{
		ar(MAKE_NVP(Parent), MAKE_NVP(Parent));
	}

	REFLECT(RelationshipComponent)
	{
		BEGIN_REFLECT(RelationshipComponent)
		REFLECT_PROPERTY_READ_ONLY("Parent", Parent) REFLECT_PROPERTY_READ_ONLY("Children", Children) COMPONENT_IMPL();
	}
} // namespace BHive