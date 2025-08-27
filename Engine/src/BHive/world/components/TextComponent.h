#pragma once

#include "World/Component.h"
#include "renderers/QuadRenderer.h"

namespace BHive
{
	struct TextComponent : public Component
	{
		float Size = 10.f;

		FTextParams Params{};

		std::string Text;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(TextComponent)
} // namespace BHive