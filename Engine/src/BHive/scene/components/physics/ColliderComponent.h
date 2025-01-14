#pragma once

#include "physics/PhysicsMaterial.h"
#include "gfx/Color.h"
#include "CollisionChannel.h"
#include "core/EnumAsByte.h"
#include "scene/components/ShapeComponent.h"


namespace BHive
{
	class Entity;

	DECLARE_EVENT(OnCollison, struct ColliderComponent *, Entity *, Entity *);
	DECLARE_EVENT(OnTrigger, struct ColliderComponent *, Entity *, Entity *);
	DECLARE_EVENT(OnHit, const glm::vec3 &, const glm::vec3 &, float);

	struct BHIVE ColliderComponent : public ShapeComponent
	{
		bool mCollisionEnabled{true};

		glm::vec3 mOffset{0.0f};

		Color mColor{0xFFFF0000};

		bool mIsTrigger = false;

		ECollisionChannel mCollisionChannel = CollisionChannel_0;

		TEnumAsByte<ECollisionChannel> mCollisionChannelMasks = CollisionChannel_All;

		TAssetHandle<PhysicsMaterial> mPhysicsMaterial;

		OnCollisonEvent OnCollisionEnter;
		OnCollisonEvent OnCollisionExit;
		OnCollisonEvent OnCollisionStay;

		OnTriggerEvent OnTriggerEnter;
		OnTriggerEvent OnTriggerExit;
		OnTriggerEvent OnTriggerStay;

		OnHitEvent OnRaycastHit;

		void OnBegin() override;
		void OnEnd() override;

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		virtual void *GetCollisionShape(const FTransform &world_transform) = 0;
		virtual void ReleaseCollisionShape() = 0;

		REFLECTABLEV(ShapeComponent)

	protected:
		void *mCollider = nullptr;
		void *mCollisionShape = nullptr;
	};

	REFLECT_EXTERN(ColliderComponent)

} // namespace BHive