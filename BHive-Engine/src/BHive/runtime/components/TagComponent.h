#pragma once

#include "runtime/Component.h"
#include "core/UUID.h"

namespace BHive
{
	struct BHIVE_API TagComponent : public Component
	{
		TagComponent() = default;
		TagComponent(const TagComponent &) = default;

		std::string Name = "New Object";

		uint16_t Group = 0;

		const UUID &GetID() const { return mID; }

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Component)

	private:
		UUID mID;
	};

	REFLECT_EXTERN(TagComponent)
} // namespace BHive