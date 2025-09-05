#pragma once

#include "World/Component.h"
#include "core/UUID.h"

namespace BHive
{
	struct GameObject;

	using ChildrenList = std::unordered_set<UUID>;

	struct BHIVE_API RelationshipComponent : public Component
	{
		RelationshipComponent() = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		UUID Parent = NullID;
		ChildrenList Children;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(RelationshipComponent)
} // namespace BHive