#pragma once

#include "Component.h"
#include "core/UUID.h"
#include "objects/GameObject.h"

namespace BHive
{
	DECLARE_CLASS()
	struct IDComponent : public Component
	{
		DECLARE_CONSTRUCTOR()
		IDComponent() = default;
		IDComponent(const IDComponent &) = default;

		DECLARE_PROPERTY(ReadOnly)
		UUID ID;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override { ar(ID); }
		virtual void Load(cereal::BinaryInputArchive &ar) override { ar(ID); }

		REFLECTABLEV(Component)
	};

	REFLECT(IDComponent)
	{
		BEGIN_REFLECT(IDComponent) COMPONENT_IMPL();
	}
} // namespace BHive