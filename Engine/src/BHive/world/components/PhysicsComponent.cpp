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

		if (auto rb = Cast<physx::PxRigidDynamic>(mRigidBodyInstance))
		{
			auto object = GetOwner();
			auto t = object->GetWorldTransform();

			auto global_pose = rb->getGlobalPose();
			auto scale = t.get_scale();
			auto global = physics::utils::Convert(global_pose);
			global.set_scale(scale);

			object->SetWorldTransform(global);
		}
	}

	void PhysicsComponent::SetGravityEnabled(bool enabled)
	{
		if (auto rb = Cast<physx::PxRigidDynamic>(mRigidBodyInstance))
		{
			rb->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, enabled);
		}
	}

	void PhysicsComponent::ApplyForce(const glm::vec3 &force)
	{
		if (auto rb = Cast<physx::PxRigidDynamic>(mRigidBodyInstance))
		{
			rb->addForce({force.x, force.y, force.z});
		}
	}

	void PhysicsComponent::SetBodyType(EBodyType type)
	{
	}

	void PhysicsComponent::SetVelocity(const glm::vec3 &velocity)
	{
		if (auto rb = Cast<physx::PxRigidDynamic>(mRigidBodyInstance))
		{
			rb->setLinearVelocity({velocity.x, velocity.y, velocity.z});
		}
	}

	glm::vec3 PhysicsComponent::GetVelocity() const
	{
		if (auto rb = Cast<physx::PxRigidDynamic>(mRigidBodyInstance))
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
		if (auto rb = Cast<physx::PxRigidDynamic>(mRigidBodyInstance))
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