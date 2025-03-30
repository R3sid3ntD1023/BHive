#pragma once

#include "Component.h"
#include "core/UUID.h"
#include "objects/GameObject.h"

namespace BHive
{
	REFLECT_CLASS()
	struct IDComponent : public Component
	{
		REFLECT_CONSTRUCTOR()
		IDComponent() = default;
		IDComponent(const IDComponent &) = default;

		REFLECT_PROPERTY(ReadOnly)
		UUID ID;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override { ar(ID); }
		virtual void Load(cereal::BinaryInputArchive &ar) override { ar(ID); }

		REFLECTABLE_CLASS(Component)
	};

} // namespace BHive