#pragma once

#include "scene/components/ShapeComponent.h"
#include "physics/PhysicsMaterial.h"
#include "gfx/Color.h"

namespace BHive
{
	class Entity;

	DECLARE_EVENT(OnCollison, struct ColliderComponent*, Entity*, Entity*);
	DECLARE_EVENT(OnTrigger, struct ColliderComponent*, Entity*, Entity*);

	struct BHIVE ColliderComponent : public ShapeComponent
	{
		bool mCollisionEnabled{true};

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

		void OnBegin() override;
		void OnEnd() override;

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);


		virtual void* GetCollisionShape(const FTransform& world_transform) = 0;
		virtual void ReleaseCollisionShape() = 0;

		REFLECTABLEV(ShapeComponent)

	protected:

		void* mCollider = nullptr;
		void* mCollisionShape = nullptr;
	};

	
	REFLECT_EXTERN(ColliderComponent)

}