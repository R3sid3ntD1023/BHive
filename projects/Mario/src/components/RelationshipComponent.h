#pragma once

#include "core/Core.h"
#include "core/UUID.h"

namespace BHive
{
	struct RelationshipComponent
	{
		UUID Parent = UUID::Null;
		std::unordered_set<UUID> Children;
	};

} // namespace BHive