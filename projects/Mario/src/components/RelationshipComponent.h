#pragma once

#include "Component.h"
#include "core/Core.h"
#include "core/UUID.h"

namespace BHive
{
	struct RelationshipComponent : public Component
	{
		UUID Parent = UUID::Null;
		std::unordered_set<UUID> Children;
	};

} // namespace BHive