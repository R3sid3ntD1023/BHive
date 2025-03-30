#pragma once

#include "Component.h"
#include "core/Core.h"
#include "core/UUID.h"
#include "objects/GameObject.h"

namespace BHive
{
	REFLECT_CLASS()
	struct RelationshipComponent : public Component
	{
		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent &) = default;

		REFLECT_PROPERTY(ReadOnly)
		UUID Parent = NullID;

		REFLECT_PROPERTY(ReadOnly)
		std::unordered_set<UUID> Children;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override { ar(Parent, Children); };
		virtual void Load(cereal::BinaryInputArchive &ar) override { ar(Parent, Children); }

		REFLECTABLE_CLASS(Component)
	};

} // namespace BHive