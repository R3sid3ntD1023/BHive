#pragma once

#include "ColliderComponent.h"

namespace BHive
{
	struct BHIVE BoxComponent : public ColliderComponent
	{
		glm::vec3 mExtents{0.5f};

		virtual AABB GetBoundingBox() const { return FBox{{}, mExtents}; }

		virtual void *GetCollisionShape(const FTransform &world_transform) override;
		virtual void ReleaseCollisionShape() override;

		virtual void OnRender(class SceneRenderer *renderer);

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(ColliderComponent)
	};

	REFLECT_EXTERN(BoxComponent);
} // namespace BHive