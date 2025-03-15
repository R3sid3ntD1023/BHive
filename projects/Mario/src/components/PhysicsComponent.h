#pragma once

#include "core/EnumAsByte.h"
#include "core/Core.h"
#include "physics/LockAxis.h"
#include "Component.h"
#include <glm/glm.hpp>

namespace BHive
{

	enum class EBodyType : int
	{
		Static,
		Kinematic,
		Dynamic
	};

	struct PhysicsSettings
	{
		bool PhysicsEnabled{true};

		EBodyType BodyType = EBodyType::Static;

		float Mass = 1.0f;

		float AngularDamping = 0.0f;

		float LinearDamping = 0.0f;

		TEnumAsByte<ELockAxis> LinearLockAxis = NoAxis;

		TEnumAsByte<ELockAxis> AngularLockAxis = NoAxis;

		bool GravityEnabled = true;
	};

	struct PhysicsComponent : public Component
	{
		PhysicsSettings Settings;

		void Begin() override;
		void Update(float) override;
		void End() override;

		void ApplyForce(const glm::vec3 &force);

		void SetBodyType(EBodyType type);

		void SetVelocity(const glm::vec3 &velocity);

		glm::vec3 GetVelocity() const;

		void SetRigidBody(void *rigidbody);

		void *GetRigidBody() { return mRigidBodyInstance; }

		EBodyType GetBodyType() const;

	private:
		void *mRigidBodyInstance = nullptr;
	};
} // namespace BHive