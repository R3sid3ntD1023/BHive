#include "ColliderComponent.h"
#include "Physics/PhysicsCore.h"

namespace BHive
{
	void ColliderComponent::Begin()
	{
		auto object = GetOwner();
		auto &physc = object->GetPhysicsComponent();
		auto rb = physc.GetRigidBody();

		CreateCollsionShape(rb, object->GetTransform());
	}

	void ColliderComponent::Update(float)
	{
	}

	void ColliderComponent::End()
	{
		auto object = GetOwner();
		auto &physc = object->GetPhysicsComponent();
		auto rb = physc.GetRigidBody();
		ReleaseCollisionShape(rb);
	}
	void ColliderComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(CollisionEnabled, Offset, Color, IsTrigger, CollisionChannel, CollisionChannelMasks,
		   TAssetHandle(PhysicsMaterial));
	}

	void ColliderComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(CollisionEnabled, Offset, Color, IsTrigger, CollisionChannel, CollisionChannelMasks,
		   TAssetHandle(PhysicsMaterial));
	}

	void BHive::ColliderComponent::CreateCollsionShape(void *rb, const FTransform &transform)
	{
		mCollisionShape = (rp3d::CollisionShape *)GetCollisionShape(transform);
		if (!mCollisionShape)
			return;

		auto offset = Offset * transform.get_scale();
		auto offset_ = rp3d::Transform({offset.x, offset.y, offset.z}, rp3d::Quaternion::identity());
		auto collider = ((rp3d::RigidBody *)rb)->addCollider((rp3d::CollisionShape *)mCollisionShape, offset_);
		collider->setUserData(this);
		collider->setCollisionCategoryBits(CollisionChannel);
		collider->setCollideWithMaskBits(CollisionChannelMasks);
		collider->setIsTrigger(IsTrigger);

		if (PhysicsMaterial)
		{
			auto &material = collider->getMaterial();
			material.setFrictionCoefficient(PhysicsMaterial->mFrictionCoefficient);
			material.setBounciness(PhysicsMaterial->mBounciness);
			material.setMassDensity(PhysicsMaterial->mMassDensity);
		}

		mCollider = collider;
	}

	void ColliderComponent::ReleaseCollisionShape(void *rb)
	{
		if (CollisionEnabled && mCollider)
		{
			((rp3d::RigidBody *)rb)->removeCollider((rp3d::Collider *)mCollider);
			OnReleaseCollisionShape();
			mCollider = nullptr;
		}
	}

	REFLECT(ColliderComponent)
	{
		BEGIN_REFLECT(ColliderComponent)
		REFLECT_PROPERTY(CollisionEnabled)
		REFLECT_PROPERTY(Offset)
		REFLECT_PROPERTY(Color)
		REFLECT_PROPERTY(IsTrigger)
		REFLECT_PROPERTY(CollisionChannel)
		REFLECT_PROPERTY(CollisionChannelMasks)
		REFLECT_PROPERTY(PhysicsMaterial);
	}

	REFLECT(ECollisionChannel)
	{
		BEGIN_REFLECT_ENUM(ECollisionChannel)
		(ENUM_VALUE(CollisionChannel_None), ENUM_VALUE(CollisionChannel_0), ENUM_VALUE(CollisionChannel_1),
		 ENUM_VALUE(CollisionChannel_2), ENUM_VALUE(CollisionChannel_3), ENUM_VALUE(CollisionChannel_4),
		 ENUM_VALUE(CollisionChannel_5), ENUM_VALUE(CollisionChannel_6), ENUM_VALUE(CollisionChannel_7),
		 ENUM_VALUE(CollisionChannel_8), ENUM_VALUE(CollisionChannel_9), ENUM_VALUE(CollisionChannel_10),
		 ENUM_VALUE(CollisionChannel_11), ENUM_VALUE(CollisionChannel_12), ENUM_VALUE(CollisionChannel_13),
		 ENUM_VALUE(CollisionChannel_14), ENUM_VALUE(CollisionChannel_All));
	}
} // namespace BHive