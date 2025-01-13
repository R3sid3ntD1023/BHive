#pragma once

#include "ColliderComponent.h"

namespace BHive
{
	struct BHIVE CapsuleComponent : public ColliderComponent
	{
		float mHeight = 2.0f;
		float mRadius = 1.0f;

		virtual AABB GetBoundingBox() const { return FBox{{}, {}}; }

		virtual void *GetCollisionShape(const FTransform &world_transform) override;
		virtual void ReleaseCollisionShape() override;

		virtual void OnRender(class SceneRenderer *renderer);

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(ColliderComponent)
	};

	REFLECT_EXTERN(CapsuleComponent);
} // namespace BHive