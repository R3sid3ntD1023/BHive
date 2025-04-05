#pragma once

#include "Component.h"
#include "core/UUID.h"
#include "GameObject.h"

namespace BHive
{
	struct IDComponent : public Component
	{
		IDComponent() = default;
		IDComponent(const IDComponent &) = default;

		UUID ID;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override { ar(ID); }
		virtual void Load(cereal::BinaryInputArchive &ar) override { ar(ID); }

		REFLECTABLEV(Component)
	};

	REFLECT(IDComponent)
	{
		BEGIN_REFLECT(IDComponent) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(ID) COMPONENT_IMPL();
	}
} // namespace BHive