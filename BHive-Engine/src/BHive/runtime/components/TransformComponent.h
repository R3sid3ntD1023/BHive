#pragma once

#include "runtime/Component.h"

namespace BHive
{
	struct BHIVE_API TransformComponent : public Component
	{
		TransformComponent() = default;
		TransformComponent(const TransformComponent &) = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		FTransform Transform{};

		void SetTransform(const FTransform &transform)
		{
			if (Transform == transform)
				return;

			Transform = transform;
			MarkDirty();
		}

		REFLECTABLEV(Component);
	};

	REFLECT_EXTERN(TransformComponent)
} // namespace BHive