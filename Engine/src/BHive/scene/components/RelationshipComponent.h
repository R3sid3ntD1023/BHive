#pragma once

#include "core/UUID.h"
#include <unordered_set>

namespace BHive
{
	class RelationshipComponent
	{
	public:
		using Children = std::unordered_set<UUID>;

		void Serialize(StreamWriter& ar) const;

		void Deserialize(StreamReader& ar);

		void SetParentID(const UUID& parent);

		void RemoveFromParent();

		void AddChild(const UUID& child);

		void RemoveChild(const UUID& child);

		const UUID& GetParentID() const { return mParentID; }

		const Children& GetChildren() const { return mChildIDs; }

		bool HasChildren() const { return mChildIDs.size() > 0; }

		bool HasParent() const { return (bool)mParentID; }

	private:
		UUID mParentID = 0;
		Children mChildIDs;
	};
}