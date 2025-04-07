#include "GameObject.h"
#include "physics/PhysicsCore.h"
#include "physics/PhysicsUtils.h"
#include "PhysicsComponent.h"

namespace BHive
{
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

	REFLECT(PhysicsSettings)
	{
		{
			BEGIN_REFLECT_ENUM(ELockAxis)
			(ENUM_VALUE(NoAxis), ENUM_VALUE(AxisX), ENUM_VALUE(AxisY), ENUM_VALUE(AxisZ));
		}
		{
			BEGIN_REFLECT_ENUM(EBodyType)
			(ENUM_VALUE(Static), ENUM_VALUE(Kinematic), ENUM_VALUE(Dynamic));
		}
		{
			BEGIN_REFLECT(PhysicsSettings)
			REFLECT_PROPERTY(PhysicsEnabled)
			REFLECT_PROPERTY(BodyType)
			REFLECT_PROPERTY(Mass)
			REFLECT_PROPERTY(AngularDamping)
			REFLECT_PROPERTY(LinearDamping)
			REFLECT_PROPERTY(LinearLockAxis)
			REFLECT_PROPERTY(AngularLockAxis)
			REFLECT_PROPERTY(GravityEnabled);
		}
	}

	REFLECT(PhysicsComponent)
	{
		BEGIN_REFLECT(PhysicsComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY(Settings) COMPONENT_IMPL();
	}
} // namespace BHive