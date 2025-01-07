#include "ColliderComponents.h"
#include "scene/SceneRenderer.h"
#include "scene/Entity.h"
#include <reactphysics3d/reactphysics3d.h>
#include "physics/PhysicsContext.h"

namespace BHive
{
    void ColliderComponent::Serialize(StreamWriter& ar) const
    {
        ShapeComponent::Serialize(ar);

        ar(mOffset, mIsTrigger, mColor, mPhysicsMaterial);
    }

    void ColliderComponent::Deserialize(StreamReader& ar)
    {
        ShapeComponent::Deserialize(ar);

        ar(mOffset, mIsTrigger, mColor, mPhysicsMaterial);
    }

    void BoxComponent::OnBegin()
    {
        auto rb = (rp3d::RigidBody*)GetOwner()->GetPhysicsComponent().GetRigidBody();

        if (rb)
        {
            auto transform = GetWorldTransform();
            auto extents = mExtents * 2.f * transform.get_scale();
            auto offset = mOffset * transform.get_scale();

            auto offset_ = rp3d::Transform({offset.x, offset.y, offset.z},
             								rp3d::Quaternion::identity());

            auto boxshape = PhysicsContext::get_context().createBoxShape({extents.x * .5f, extents.y * .5f, extents.z * .5f});
            auto collider = rb->addCollider(boxshape, offset_);
            collider->setUserData(this);

            collider->setIsTrigger(mIsTrigger);

            if (mPhysicsMaterial)
            {
             	auto &material = collider->getMaterial();
             	material.setFrictionCoefficient(mPhysicsMaterial->mFrictionCoefficient);
             	material.setBounciness(mPhysicsMaterial->mBounciness);
             	material.setMassDensity(mPhysicsMaterial->mMassDensity);
            }

            mCollider = collider;
            mShape = boxshape;
        }
    }

    void BoxComponent::OnEnd()
    {
        if (mCollider)
        {
            auto rb = (rp3d::RigidBody*)GetOwner()->GetPhysicsComponent().GetRigidBody();
            rb->removeCollider((rp3d::Collider*)mCollider);
            PhysicsContext::get_context().destroyBoxShape((rp3d::BoxShape*)mShape);
        }
    }

    void BoxComponent::OnRender(SceneRenderer *renderer)
    {
        LineRenderer::DrawBox(mExtents, mOffset, mColor, GetWorldTransform());
    }

    void BoxComponent::Serialize(StreamWriter& ar) const
    {
        ColliderComponent::Serialize(ar);
        ar(mExtents);
    }

    void BoxComponent::Deserialize(StreamReader& ar)
    {
        ColliderComponent::Deserialize(ar);
        ar(mExtents);
    }

    void SphereComponent::OnBegin()
    {
        auto rb = (rp3d::RigidBody*)GetOwner()->GetPhysicsComponent().GetRigidBody();

        if (rb)
        {
            auto transform = GetWorldTransform();
            auto extents = mRadius * glm::compMax(transform.get_scale());
            auto offset = mOffset * transform.get_scale();

            auto offset_ = rp3d::Transform({ offset.x, offset.y, offset.z },
                rp3d::Quaternion::identity());

            auto shape = PhysicsContext::get_context().createSphereShape(extents);
            auto collider = rb->addCollider(shape, offset_);
            collider->setUserData(this);

            collider->setIsTrigger(mIsTrigger);

            if (mPhysicsMaterial)
            {
                auto& material = collider->getMaterial();
                material.setFrictionCoefficient(mPhysicsMaterial->mFrictionCoefficient);
                material.setBounciness(mPhysicsMaterial->mBounciness);
                material.setMassDensity(mPhysicsMaterial->mMassDensity);
            }

            mCollider = collider;
            mShape = shape;
        }
    }

    void SphereComponent::OnEnd()
    {
        if (mCollider)
        {
            auto rb = (rp3d::RigidBody*)GetOwner()->GetPhysicsComponent().GetRigidBody();
            rb->removeCollider((rp3d::Collider*)mCollider);
            PhysicsContext::get_context().destroySphereShape((rp3d::SphereShape*)mShape);
        }
    }

    void SphereComponent::OnRender(SceneRenderer *renderer)
    {
        LineRenderer::DrawSphere(mRadius, 32, mOffset, mColor, GetWorldTransform());
    }

    void SphereComponent::Serialize(StreamWriter& ar) const
    {
        ColliderComponent::Serialize(ar);
        ar(mRadius);
    }

    void SphereComponent::Deserialize(StreamReader& ar)
    {
        ColliderComponent::Deserialize(ar);
        ar(mRadius);
    }

      
    void CapsuleComponent::OnBegin()
	{
		auto rb = (rp3d::RigidBody*)GetOwner()->GetPhysicsComponent().GetRigidBody();

		if (rb)
		{
			auto transform = GetWorldTransform();
			auto extents = mRadius * glm::compMax(transform.get_scale());
            auto height = mHeight * glm::compMax(transform.get_scale());
			auto offset = mOffset * transform.get_scale();

			auto offset_ =
				rp3d::Transform({offset.x, offset.y, offset.z}, rp3d::Quaternion::identity());

			auto shape = PhysicsContext::get_context().createCapsuleShape(extents, height);
			
			auto collider = rb->addCollider(shape, offset_);
			collider->setUserData(this);

			collider->setIsTrigger(mIsTrigger);

			if (mPhysicsMaterial)
			{
				auto& material = collider->getMaterial();
				material.setFrictionCoefficient(mPhysicsMaterial->mFrictionCoefficient);
				material.setBounciness(mPhysicsMaterial->mBounciness);
				material.setMassDensity(mPhysicsMaterial->mMassDensity);
			}

			mCollider = collider;
			mShape = shape;
		}
	}

    void CapsuleComponent::OnEnd() {
    
        if (mCollider)
		{
			auto rb = (rp3d::RigidBody*)GetOwner()->GetPhysicsComponent().GetRigidBody();
			rb->removeCollider((rp3d::Collider*)mCollider);
			PhysicsContext::get_context().destroyCapsuleShape((rp3d::CapsuleShape*)mShape);
		}
	}

    void CapsuleComponent::OnRender(SceneRenderer* renderer)
	{
		LineRenderer::DrawCapsule(mRadius, mHeight, 16, mOffset, mColor, GetWorldTransform());
    }

    void CapsuleComponent::Serialize(StreamWriter& ar) const 
    {
        ColliderComponent::Serialize(ar);
        ar(mHeight, mRadius);
    }

    void CapsuleComponent::Deserialize(StreamReader& ar) 
    {
        ColliderComponent::Deserialize(ar);
        ar(mHeight, mRadius);
    }

    REFLECT(ColliderComponent)
    {
		BEGIN_REFLECT(ColliderComponent)
		REFLECT_PROPERTY("Offset", mOffset)
		REFLECT_PROPERTY("Color", mColor)
		REFLECT_PROPERTY("IsTrigger", mIsTrigger)
		REFLECT_PROPERTY("PhysicsMaterial", mPhysicsMaterial);
    }

    REFLECT(BoxComponent)
    {
		BEGIN_REFLECT(BoxComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Extents", mExtents);
    }

    REFLECT(SphereComponent)
    {
		BEGIN_REFLECT(SphereComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Radius", mRadius);
    }

    REFLECT(CapsuleComponent)
    {
		BEGIN_REFLECT(CapsuleComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Radius", mRadius) REFLECT_PROPERTY("Height", mHeight);
    }

    }  // namespace  BHive
