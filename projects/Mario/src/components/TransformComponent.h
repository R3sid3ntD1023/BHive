#pragma once

#include "Component.h"
#include "math/Transform.h"
#include "objects/GameObject.h"

namespace BHive
{
	DECLARE_CLASS()
	struct TransformComponent : public Component
	{
		DECLARE_CONSTRUCTOR()
		TransformComponent() = default;
		TransformComponent(const TransformComponent &) = default;

		DECLARE_PROPERTY()
		FTransform Transform{};

		virtual void Save(cereal::BinaryOutputArchive &ar) const override { ar(Transform); }
		virtual void Load(cereal::BinaryInputArchive &ar) override { ar(Transform); }

		REFLECTABLEV(Component)
	};

	REFLECT(TransformComponent)
	{
		BEGIN_REFLECT(TransformComponent) COMPONENT_IMPL();
	}

} // namespace BHive