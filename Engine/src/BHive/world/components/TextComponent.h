#pragma once

#include "Component.h"
#include "renderers/QuadRenderer.h"

namespace BHive
{
	struct TextComponent : public Component
	{
		virtual void Render() override;

		float Size = 10.f;

		FTextParams Params{};

		std::string Text;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(TextComponent)
}