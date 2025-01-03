#include "RelationshipComponent.h"

namespace BHive
{
	void RelationshipComponent::Serialize(StreamWriter& ar) const
	{
		ar(mParentID, mChildIDs);
	}

	void RelationshipComponent::Deserialize(StreamReader& ar)
	{
		ar(mParentID, mChildIDs);
	}

	void RelationshipComponent::SetParentID(const UUID& parent)
	{
		mParentID = parent;
	}

	void RelationshipComponent::RemoveFromParent()
	{
		mParentID = 0;
	}

	void RelationshipComponent::AddChild(const UUID& child)
	{
		mChildIDs.insert(child);
	}

	void RelationshipComponent::RemoveChild(const UUID& child)
	{
		mChildIDs.erase(child);
	}
}