#include "ColliderComponent.h"
#include "physics/PhysicsContext.h"
#include "physics/PhysicsUtils.h"
#include "scene/Entity.h"
#include <reactphysics3d/reactphysics3d.h>

namespace BHive
{
	void ColliderComponent::OnBegin()
	{
		auto rb = (rp3d::RigidBody *)GetOwner()->GetPhysicsComponent().GetRigidBody();
		if (rb && mCollisionEnabled)
		{
			auto transform = GetWorldTransform();
			auto shape = (rp3d::CollisionShape *)GetCollisionShape(transform);
			if (!shape)
				return;

			auto offset = mOffset * transform.get_scale();
			auto offset_ = rp3d::Transform({offset.x, offset.y, offset.z}, rp3d::Quaternion::identity());
			auto collider = rb->addCollider(shape, offset_);
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
	}
	void ColliderComponent::OnEnd()
	{
		if (mCollisionEnabled && mCollider)
		{
			auto rb = (rp3d::RigidBody *)GetOwner()->GetPhysicsComponent().GetRigidBody();
			rb->removeCollider((rp3d::Collider *)mCollider);
			ReleaseCollisionShape();
		}
	}

	void ColliderComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ShapeComponent::Save(ar);

		ar(mCollisionEnabled, mCollisionChannel, mCollisionChannelMasks, mOffset, mIsTrigger, mColor, mPhysicsMaterial);
	}

	void ColliderComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ShapeComponent::Load(ar);

		ar(mCollisionEnabled, mCollisionChannel, mCollisionChannelMasks, mOffset, mIsTrigger, mColor, mPhysicsMaterial);
	}

	REFLECT(ColliderComponent)
	{
		BEGIN_REFLECT(ColliderComponent)
		REFLECT_PROPERTY("IsTrigger", mIsTrigger)
		REFLECT_PROPERTY("Collision Enabled", mCollisionEnabled)
		REFLECT_PROPERTY("Collsion Category", mCollisionChannel)
		REFLECT_PROPERTY("Collsion Category Masks", mCollisionChannelMasks)
		REFLECT_PROPERTY("PhysicsMaterial", mPhysicsMaterial)
		REFLECT_PROPERTY("Offset", mOffset)
		REFLECT_PROPERTY("Color", mColor);
	}

} // namespace  BHive
