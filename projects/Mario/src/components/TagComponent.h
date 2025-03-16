#pragma once

#include "Component.h"
#include "core/Core.h"

namespace BHive
{
	struct TagComponent : public Component
	{
		TagComponent() = default;
		TagComponent(const TagComponent &) = default;
		TagComponent(const std::string &name, uint16_t groups = 0)
			: Name(name),
			  Groups(groups)
		{
		}

		std::string Name = "New Object";
		uint16_t Groups = 0x00;
	};

} // namespace BHive