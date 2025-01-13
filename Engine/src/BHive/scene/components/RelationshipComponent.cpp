#include "RelationshipComponent.h"

namespace BHive
{

	void RelationshipComponent::SetParentID(const UUID &parent)
	{
		mParentID = parent;
	}

	void RelationshipComponent::RemoveFromParent()
	{
		mParentID = 0;
	}

	void RelationshipComponent::AddChild(const UUID &child)
	{
		mChildIDs.insert(child);
	}

	void RelationshipComponent::RemoveChild(const UUID &child)
	{
		mChildIDs.erase(child);
	}
} // namespace BHive