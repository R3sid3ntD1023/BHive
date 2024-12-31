#pragma once

#include "ShapeComponent.h"
#include "physics/PhysicsMaterial.h"
#include "gfx/Color.h"

namespace BHive
{

	struct BHIVE ColliderComponent : public ShapeComponent
	{
		glm::vec3 mOffset{0.0f};

		Color mColor{0xFFFF0000};

		bool mIsTrigger = false;

		TAssetHandle<PhysicsMaterial> mPhysicsMaterial;

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ShapeComponent)
	};

	struct BHIVE SphereComponent : public ColliderComponent
	{
		float mRadius = 0.5f;

		virtual AABB GetBoundingBox() const { return FSphere{{}, mRadius}; }

		virtual void OnRender(class SceneRenderer *renderer);

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ColliderComponent)
	};

	struct BHIVE BoxComponent : public ColliderComponent
	{
		glm::vec3 mExtents{0.5f};

		virtual AABB GetBoundingBox() const { return FBox{{}, mExtents}; }

		virtual void OnRender(class SceneRenderer *renderer);

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ColliderComponent)
	};

	REFLECT_EXTERN(ColliderComponent)
	REFLECT_EXTERN(BoxComponent)
	REFLECT_EXTERN(SphereComponent)

}