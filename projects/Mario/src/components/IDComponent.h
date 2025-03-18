#pragma once

#include "Component.h"
#include "core/UUID.h"

namespace BHive
{
	struct IDComponent : public Component
	{
		UUID ID;

		REFLECTABLEV(Component)
	};

	REFLECT(IDComponent)
	{
		BEGIN_REFLECT(IDComponent) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(ID);
	}
} // namespace BHive