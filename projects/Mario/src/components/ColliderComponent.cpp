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
	void BHive::ColliderComponent::CreateCollsionShape(void *rb, const FTransform &transform)
	{
		mCollisionShape = (rp3d::CollisionShape *)GetCollisionShape(transform);
		if (!mCollisionShape)
			return;

		auto offset = mOffset * transform.get_scale();
		auto offset_ = rp3d::Transform({offset.x, offset.y, offset.z}, rp3d::Quaternion::identity());
		auto collider = ((rp3d::RigidBody *)rb)->addCollider((rp3d::CollisionShape *)mCollisionShape, offset_);
		collider->setUserData(this);
		collider->setCollisionCategoryBits(mCollisionChannel);
		collider->setCollideWithMaskBits(mCollisionChannelMasks);
		collider->setIsTrigger(mIsTrigger);

		if (mPhysicsMaterial)
		{
			auto &material = collider->getMaterial();
			material.setFrictionCoefficient(mPhysicsMaterial->mFrictionCoefficient);
			material.setBounciness(mPhysicsMaterial->mBounciness);
			material.setMassDensity(mPhysicsMaterial->mMassDensity);
		}

		mCollider = collider;
	}

	void ColliderComponent::ReleaseCollisionShape(void *rb)
	{
		if (mCollisionEnabled && mCollider)
		{
			((rp3d::RigidBody *)rb)->removeCollider((rp3d::Collider *)mCollider);
			OnReleaseCollisionShape();
			mCollider = nullptr;
		}
	}
} // namespace BHive