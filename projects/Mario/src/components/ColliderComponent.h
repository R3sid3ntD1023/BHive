#pragma once

#include "core/Core.h"
#include "core/EnumAsByte.h"
#include "core/EventDelegate.h"
#include "gfx/Color.h"
#include "math/Transform.h"
#include "objects/GameObject.h"
#include "physics/CollisionChannel.h"
#include "physics/PhysicsMaterial.h"
#include "RuntimeEventDelegate.h"

namespace BHive
{
	struct GameObject;

	DECLARE_EVENT(OnCollison, struct ColliderComponent *, GameObject *);
	DECLARE_EVENT(OnTrigger, struct ColliderComponent *, GameObject *);
	DECLARE_EVENT(OnHit, const glm::vec3 &, const glm::vec3 &, float);

	DECLARE_CLASS()
	struct ColliderComponent : public Component
	{
		ColliderComponent() = default;
		ColliderComponent(const ColliderComponent &other) = default;

		DECLARE_PROPERTY()
		bool CollisionEnabled{true};

		DECLARE_PROPERTY()
		glm::vec3 Offset{0.0f};

		DECLARE_PROPERTY()
		FColor Color{0xffff00ff};

		DECLARE_PROPERTY()
		bool IsTrigger = false;

		DECLARE_PROPERTY()
		ECollisionChannel CollisionChannel = CollisionChannel_0;

		DECLARE_PROPERTY()
		TEnumAsByte<ECollisionChannel> CollisionChannelMasks = CollisionChannel_All;

		DECLARE_PROPERTY()
		Ref<PhysicsMaterial> PhysicsMaterial;

		OnCollisonEvent OnCollisionEnter;
		OnCollisonEvent OnCollisionExit;
		OnCollisonEvent OnCollisionStay;

		OnTriggerEvent OnTriggerEnter;
		OnTriggerEvent OnTriggerExit;
		OnTriggerEvent OnTriggerStay;

		OnHitEvent OnRaycastHit;

		DECLARE_PROPERTY()
		RuntimeEventDelegate Delegate;

		void Begin() override;
		void Update(float) override;
		void End() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		void CreateCollsionShape(void *rb, const FTransform &transform);
		void ReleaseCollisionShape(void *rb);

		virtual void *GetCollisionShape(const FTransform &world_transform) = 0;
		virtual void OnReleaseCollisionShape() = 0;

	protected:
		void *mCollider = nullptr;
		void *mCollisionShape = nullptr;

		REFLECTABLE_CLASS(Component)
	};

} // namespace BHive