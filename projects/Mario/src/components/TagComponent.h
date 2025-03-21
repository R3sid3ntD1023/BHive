#pragma once

#include "Component.h"
#include "core/Core.h"
#include "objects/GameObject.h"

namespace BHive
{
	struct TagComponent : public Component
	{
		std::string Name = "New Object";
		uint16_t Groups = 0x00;

		TagComponent() = default;
		TagComponent(const TagComponent &) = default;
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
		BEGIN_REFLECT(TagComponent) REFLECT_PROPERTY(Name) REFLECT_PROPERTY(Groups) COMPONENT_IMPL();
	}

} // namespace BHive