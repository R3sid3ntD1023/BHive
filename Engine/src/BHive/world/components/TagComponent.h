#pragma once

#include "Component.h"
#include "core/UUID.h"

namespace BHive
{
	struct TagComponent : public Component
	{
		TagComponent();
		TagComponent(const TagComponent &) = default;

		std::string Name = "New Object";

		uint16_t Group = 0;

		const UUID &GetID() const { return mID; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		virtual void Save(cereal::JSONOutputArchive &ar) const;
		virtual void Load(cereal::JSONInputArchive &ar);

		REFLECTABLEV(Component)

	private:
		UUID mID;
	};

	REFLECT_EXTERN(TagComponent)
} // namespace BHive