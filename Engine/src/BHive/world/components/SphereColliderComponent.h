#pragma once

#include "ColliderComponent.h"

namespace BHive
{
	struct SphereColliderComponent : public ColliderComponent
	{
		float Radius = .5f;

		virtual void Render() override;
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		void *GetGeometry() override;

		REFLECTABLEV(ColliderComponent)
	};

	REFLECT_EXTERN(SphereColliderComponent)
} // namespace BHive