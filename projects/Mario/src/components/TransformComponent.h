#pragma once

#include "Component.h"
#include "math/Transform.h"
#include "objects/GameObject.h"

namespace BHive
{
	struct TransformComponent : public Component
	{
		TransformComponent() = default;
		TransformComponent(const TransformComponent &) = default;

		FTransform Transform{};

		virtual void Save(cereal::BinaryOutputArchive &ar) const override { ar(Transform); }
		virtual void Load(cereal::BinaryInputArchive &ar) override { ar(Transform); }

		REFLECTABLEV(Component)
	};

	REFLECT(FTransform)
	{
		BEGIN_REFLECT(FTransform)
			.property("Transform", &T::get_translation, rttr::select_overload<void(const glm::vec3 &)>(&T::set_translation))
				REFLECT_PROPERTY("Rotation", get_rotation, set_rotation)
					REFLECT_PROPERTY("Scale", get_scale, set_scale)(META_DATA(EPropertyMetaData_Default, glm::vec3(1.f)));
	}

	REFLECT(TransformComponent)
	{
		BEGIN_REFLECT(TransformComponent) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Transform) COMPONENT_IMPL();
	}

} // namespace BHive