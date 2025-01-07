#pragma once

#include "ShapeComponent.h"
#include "physics/PhysicsMaterial.h"
#include "gfx/Color.h"

namespace BHive
{
	class Entity;

	DECLARE_EVENT(OnCollison, struct ColliderComponent*, Entity*, Entity*);
	DECLARE_EVENT(OnTrigger, struct ColliderComponent*, Entity*, Entity*);

	struct BHIVE ColliderComponent : public ShapeComponent
	{
		glm::vec3 mOffset{0.0f};

		Color mColor{0xFFFF0000};

		bool mIsTrigger = false;

		TAssetHandle<PhysicsMaterial> mPhysicsMaterial;

		OnCollisonEvent OnCollisionEnter;
        OnCollisonEvent OnCollisionExit;
        OnCollisonEvent OnCollisionStay;

        OnTriggerEvent OnTriggerEnter;
        OnTriggerEvent OnTriggerExit;
        OnTriggerEvent OnTriggerStay;

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ShapeComponent)

	protected:

		void* mCollider = nullptr;
		void* mShape = nullptr;
	};

	struct BHIVE SphereComponent : public ColliderComponent
	{
		float mRadius = 0.5f;

		virtual AABB GetBoundingBox() const { return FSphere{{}, mRadius}; }

		virtual void OnBegin() override;
		virtual void OnEnd() override;
		virtual void OnRender(class SceneRenderer *renderer);

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ColliderComponent)
	};

	struct BHIVE BoxComponent : public ColliderComponent
	{
		glm::vec3 mExtents{0.5f};

		virtual AABB GetBoundingBox() const { return FBox{{}, mExtents}; }

		virtual void OnBegin() override;
		virtual void OnEnd() override;

		virtual void OnRender(class SceneRenderer *renderer);

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ColliderComponent)

	};

	struct BHIVE CapsuleComponent : public ColliderComponent {
        
		float mHeight = 2.0f;
        float mRadius = 1.0f;

        virtual AABB GetBoundingBox() const { return FBox{{}, {}}; }

        virtual void OnBegin() override;
        virtual void OnEnd() override;

        virtual void OnRender(class SceneRenderer* renderer);

        void Serialize(StreamWriter& ar) const;
        void Deserialize(StreamReader& ar);

        REFLECTABLEV(ColliderComponent)
    };

	REFLECT_EXTERN(ColliderComponent)
	REFLECT_EXTERN(BoxComponent)
	REFLECT_EXTERN(SphereComponent)
    REFLECT_EXTERN(CapsuleComponent)

}