#include "IPhysicsComponent.h"
#include "reactphysics3d/reactphysics3d.h"

namespace BHive
{
	void IPhysicsComponent::ApplyForce(const glm::vec3 &force)
	{
		if (auto rb = (rp3d::RigidBody *)mRigidBodyInstance)
		{
			rb->applyLocalForceAtCenterOfMass({force.x, force.y, force.z});
		}
	}

	void IPhysicsComponent::SetVelocity(const glm::vec3 &velocity)
	{
		if (auto rb = (rp3d::RigidBody *)mRigidBodyInstance)
		{
			rb->setLinearVelocity({velocity.x, velocity.y, velocity.z});
		}
	}

	glm::vec3 IPhysicsComponent::GetVelocity() const
	{
		if (auto rb = (rp3d::RigidBody *)mRigidBodyInstance)
		{
			auto vel = rb->getLinearVelocity();
			return {vel.x, vel.y, vel.z};
		}

		return {};
	}

}
