#pragma once

#include "ColliderComponent.h"

namespace BHive
{
	struct BHIVE SphereComponent : public ColliderComponent
	{
		float mRadius = 0.5f;

		virtual AABB GetBoundingBox() const { return FSphere{{}, mRadius}; }

		virtual void* GetCollisionShape(const FTransform& world_transform) override;
		virtual void ReleaseCollisionShape() override;
		
		virtual void OnRender(class SceneRenderer* renderer);

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ColliderComponent)
	};

	REFLECT_EXTERN(SphereComponent)
}