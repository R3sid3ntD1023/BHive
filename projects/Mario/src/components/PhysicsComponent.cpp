#include "objects/GameObject.h"
#include "physics/PhysicsCore.h"
#include "physics/PhysicsUtils.h"
#include "PhysicsComponent.h"

namespace BHive
{
	void PhysicsComponent::Begin()
	{
		if (!Settings.PhysicsEnabled)
			return;

		auto object = GetOwner();
		auto world = object->GetWorld()->GetPhysicsWorld();
		auto t = object->GetTransform();
		auto rb = world->createRigidBody(physics::utils::GetPhysicsTransform(t));
		rb->setIsDebugEnabled(true);
		rb->setUserData(object);
		rb->setMass(Settings.Mass);
		rb->setType((rp3d::BodyType)Settings.BodyType);
		rb->enableGravity(Settings.GravityEnabled);
		rb->setAngularDamping(Settings.AngularDamping);
		rb->setLinearDamping(Settings.LinearDamping);

		rb->setLinearLockAxisFactor(physics::utils::LockAxisToVextor3(Settings.LinearLockAxis));
		rb->setAngularLockAxisFactor(physics::utils::LockAxisToVextor3(Settings.AngularLockAxis));

		SetRigidBody(rb);
	}

	void PhysicsComponent::Update(float)
	{
		if (!Settings.PhysicsEnabled)
			return;

		auto rb = (rp3d::RigidBody *)mRigidBodyInstance;
		auto object = GetOwner();
		auto &t = object->GetLocalTransform();

		switch (Settings.BodyType)
		{
		case EBodyType::Dynamic:
		{
			auto &transform = rb->getTransform();
			auto scale = t.get_scale();
			t = physics::utils::GetTransform(transform, scale);
			break;
		}
		case EBodyType::Kinematic:
		{
			rb->setTransform(physics::utils::GetPhysicsTransform(t));
			break;
		}
		default:
			break;
		}
	}

	void PhysicsComponent::End()
	{
		if (!Settings.PhysicsEnabled)
			return;

		auto object = GetOwner();
		auto world = object->GetWorld()->GetPhysicsWorld();
		world->destroyRigidBody((rp3d::RigidBody *)mRigidBodyInstance);
	}

	void PhysicsComponent::ApplyForce(const glm::vec3 &force)
	{
		if (auto rb = Cast<rp3d::RigidBody>(mRigidBodyInstance))
		{
			rb->applyLocalForceAtCenterOfMass({force.x, force.y, force.z});
		}
	}

	void PhysicsComponent::SetBodyType(EBodyType type)
	{
		if (auto rb = Cast<rp3d::RigidBody>(mRigidBodyInstance))
		{
			rb->setType((rp3d::BodyType)type);
		}
	}

	void PhysicsComponent::SetVelocity(const glm::vec3 &velocity)
	{
		if (auto rb = Cast<rp3d::RigidBody>(mRigidBodyInstance))
		{
			rb->setLinearVelocity({velocity.x, velocity.y, velocity.z});
		}
	}

	glm::vec3 PhysicsComponent::GetVelocity() const
	{
		if (auto rb = Cast<rp3d::RigidBody>(mRigidBodyInstance))
		{
			auto vel = rb->getLinearVelocity();
			return {vel.x, vel.y, vel.z};
		}

		return {};
	}

	void PhysicsComponent::SetRigidBody(void *rigidbody)
	{
		mRigidBodyInstance = rigidbody;
	}

	EBodyType PhysicsComponent::GetBodyType() const
	{
		if (auto rb = Cast<rp3d::RigidBody>(mRigidBodyInstance))
		{
			return (EBodyType)rb->getType();
		}
	}

	void PhysicsComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Settings);
	}

	void PhysicsComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Settings);
	}

	
} // namespace BHive