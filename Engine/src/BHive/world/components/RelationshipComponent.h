#pragma once

#include "Component.h"
#include "core/UUID.h"

namespace BHive
{
	struct GameObject;

	using ChildrenList = std::unordered_set<UUID>;

	struct RelationshipComponent : public Component
	{
		RelationshipComponent();

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		virtual void Save(cereal::JSONOutputArchive &ar) const;
		virtual void Load(cereal::JSONInputArchive &ar);

		UUID Parent = NullID;
		ChildrenList Children;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(RelationshipComponent)
} // namespace BHive