#pragma once

#include "Component.h"
#include "math/Transform.h"

namespace BHive
{
	struct TransformComponent : public Component
	{
		TransformComponent();
		TransformComponent(const TransformComponent &) = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		virtual void Save(cereal::JSONOutputArchive &ar) const;
		virtual void Load(cereal::JSONInputArchive &ar);

		FTransform Transform{};

		REFLECTABLEV(Component);
	};

	REFLECT_EXTERN(TransformComponent)
} // namespace BHive