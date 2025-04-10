#pragma once

#include "ColliderComponent.h"

namespace BHive
{
	struct CapsuleColliderComponent : public ColliderComponent
	{
		float Radius = .5f;
		float HalfHeight = 1.f;

		virtual void Render() override;
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		virtual AABB GetBoundingBox() const { return FBox(); }

		void *GetGeometry() override;

		REFLECTABLEV(ColliderComponent)
	};

	REFLECT_EXTERN(CapsuleColliderComponent)
} // namespace BHive