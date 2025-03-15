#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "math/Transform.h"
#include "physics/PhysicsMaterial.h"
#include "gfx/Color.h"
#include "physics/CollisionChannel.h"
#include "core/EnumAsByte.h"
#include "objects/GameObject.h"

namespace BHive
{
	struct GameObject;

	DECLARE_EVENT(OnCollison, struct ColliderComponent *, GameObject *);
	DECLARE_EVENT(OnTrigger, struct ColliderComponent *, GameObject *);
	DECLARE_EVENT(OnHit, const glm::vec3 &, const glm::vec3 &, float);

	struct ColliderComponent : public Component
	{
		bool mCollisionEnabled{true};

		glm::vec3 mOffset{0.0f};

		FColor mColor{0xFFFF0000};

		bool mIsTrigger = false;

		ECollisionChannel mCollisionChannel = CollisionChannel_0;

		TEnumAsByte<ECollisionChannel> mCollisionChannelMasks = CollisionChannel_All;

		Ref<PhysicsMaterial> mPhysicsMaterial;

		OnCollisonEvent OnCollisionEnter;
		OnCollisonEvent OnCollisionExit;
		OnCollisonEvent OnCollisionStay;

		OnTriggerEvent OnTriggerEnter;
		OnTriggerEvent OnTriggerExit;
		OnTriggerEvent OnTriggerStay;

		OnHitEvent OnRaycastHit;

		void Begin() override;
		void Update(float) override;
		void End() override;

		void CreateCollsionShape(void *rb, const FTransform &transform);
		void ReleaseCollisionShape(void *rb);

		virtual void *GetCollisionShape(const FTransform &world_transform) = 0;
		virtual void OnReleaseCollisionShape() = 0;

	protected:
		void *mCollider = nullptr;
		void *mCollisionShape = nullptr;
	};
} // namespace BHive