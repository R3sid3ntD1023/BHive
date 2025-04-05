#pragma once

#include "Component.h"
#include "core/Core.h"
#include "core/UUID.h"
#include "GameObject.h"

namespace BHive
{
	struct RelationshipComponent : public Component
	{
		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent &) = default;

		UUID Parent = NullID;
		std::unordered_set<UUID> Children;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override { ar(Parent, Children); };
		virtual void Load(cereal::BinaryInputArchive &ar) override { ar(Parent, Children); }

		REFLECTABLEV(Component)
	};

	REFLECT(RelationshipComponent)
	{
		BEGIN_REFLECT(RelationshipComponent)
		REFLECT_PROPERTY_READ_ONLY("Parent", Parent) REFLECT_PROPERTY_READ_ONLY("Children", Children) COMPONENT_IMPL();
	}

} // namespace BHive