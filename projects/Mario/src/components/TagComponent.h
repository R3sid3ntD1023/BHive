#pragma once

#include "Component.h"
#include "core/Core.h"
#include "objects/GameObject.h"

namespace BHive
{
	REFLECT_CLASS()
	struct TagComponent : public Component
	{
		REFLECT_PROPERTY()
		std::string Name = "New Object";

		REFLECT_PROPERTY()
		uint16_t Groups = 0x00;

		REFLECT_CONSTRUCTOR()
		TagComponent() = default;
		TagComponent(const TagComponent &) = default;

		REFLECT_CONSTRUCTOR()
		TagComponent(const std::string &name, uint16_t groups = 0)
			: Name(name),
			  Groups(groups)
		{
		}

		virtual void Save(cereal::BinaryOutputArchive &ar) const override { ar(Name, Groups); }
		virtual void Load(cereal::BinaryInputArchive &ar) override { ar(Name, Groups); }

		REFLECTABLE_CLASS(Component)
	};

} // namespace BHive