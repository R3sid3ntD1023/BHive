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
} // namespace BHive