#pragma once

#include "Component.h"
#include "core/Core.h"
#include "objects/GameObject.h"

namespace BHive
{
	DECLARE_CLASS()
	struct TagComponent : public Component
	{
		DECLARE_PROPERTY()
		std::string Name = "New Object";

		DECLARE_PROPERTY()
		uint16_t Groups = 0x00;

		DECLARE_CONSTRUCTOR()
		TagComponent() = default;
		TagComponent(const TagComponent &) = default;

		DECLARE_CONSTRUCTOR()
		TagComponent(const std::string &name, uint16_t groups = 0)
			: Name(name),
			  Groups(groups)
		{
		}

		virtual void Save(cereal::BinaryOutputArchive &ar) const override { ar(Name, Groups); }
		virtual void Load(cereal::BinaryInputArchive &ar) override { ar(Name, Groups); }

		REFLECTABLEV(Component)
	};

	REFLECT(TagComponent)
	{
		BEGIN_REFLECT(TagComponent) COMPONENT_IMPL();
	}

} // namespace BHive