#pragma once

#include "Component.h"
#include "core/UUID.h"

namespace BHive
{
	struct IDComponent : public Component
	{
		UUID ID;
	};
} // namespace BHive