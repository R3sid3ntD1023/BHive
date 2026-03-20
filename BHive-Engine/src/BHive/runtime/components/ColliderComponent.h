#pragma once

#include "runtime/Component.h"
#include "core/EnumAsByte.h"
#include "core/EventDelegate.h"
#include "gfx/Color.h"
#include "core/math/boundingbox/AABB.h"
#include "physics/CollisionChannel.h"
#include "physics/PhysicsMaterial.h"

namespace BHive
{
	struct GameObject;

	DECLARE_EVENT(OnCollison, struct ColliderComponent *, GameObject *);
	DECLARE_EVENT(OnTrigger, struct ColliderComponent *, GameObject *);
	DECLARE_EVENT(OnHit, const glm::vec3 &, const glm::vec3 &, float);

	struct BHIVE_API ColliderComponent : public Component
	{
		ColliderComponent() = default;
		ColliderComponent(const ColliderComponent &other) = default;

		bool CollisionEnabled{true};

		glm::vec3 Offset{0.0f};

		FColor Color{0xffff00ff};

		bool IsTrigger = false;

		ECollisionChannel CollisionChannel = CollisionChannel_0;

		TEnumAsByte<ECollisionChannel> CollisionChannelMasks = CollisionChannel_All;

		Ref<PhysicsMaterial> PhysicsMaterial;

		OnCollisonEvent OnCollisionEnter;

		OnCollisonEvent OnCollisionExit;

		OnCollisonEvent OnCollisionStay;

		OnTriggerEvent OnTriggerEnter;

		OnTriggerEvent OnTriggerExit;

		OnTriggerEvent OnTriggerStay;

		OnHitEvent OnRaycastHit;

		void Begin() override;

		void End() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		void CreateCollsionShape(void *rb, const FTransform &transform);

		void ReleaseCollisionShape(void *rb);

		virtual void *GetGeometry() = 0;

	protected:
		// void *mCollider = nullptr;
		void *mCollisionShape = nullptr;
		void *mShapeMaterial = nullptr;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(ColliderComponent)

} // namespace BHive