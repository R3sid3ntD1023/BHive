#include "runtime/GameObject.h"
#include "RelationshipComponent.h"

namespace BHive
{
	void RelationshipComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Parent, Children);
	}

	void RelationshipComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Parent, Children);
	}

	REFLECT(RelationshipComponent)
	{
		BEGIN_REFLECT(RelationshipComponent)
		REFLECT_PROPERTY_READ_ONLY("Parent", Parent) REFLECT_PROPERTY_READ_ONLY("Children", Children) COMPONENT_IMPL();
	}
} // namespace BHive