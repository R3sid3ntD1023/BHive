#pragma once

#include "runtime/Component.h"
#include "gfx/renderers/QuadRenderer.h"

namespace BHive
{
	struct BHIVE_API TextComponent : public Component
	{
		float Size = 10.f;

		FTextParams Params{};

		std::string Text;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(TextComponent)
} // namespace BHive