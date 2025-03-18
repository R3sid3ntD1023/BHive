#pragma once

#include "Component.h"
#include "math/Transform.h"

namespace BHive
{
	struct TransformComponent : public Component
	{
		FTransform Transform{};

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
		BEGIN_REFLECT(TransformComponent) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Transform);
	}

} // namespace BHive