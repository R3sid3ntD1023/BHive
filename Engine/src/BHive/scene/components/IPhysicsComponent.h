#pragma once

#include "core/EnumAsByte.h"
#include "math/Math.h"
#include "physics/LockAxis.h"
#include "serialization/Serialization.h"

namespace BHive
{
	enum class EBodyType : int
	{
		Static,
		Kinematic,
		Dynamic
	};

	struct BHIVE IPhysicsComponent
	{
		bool mPhysicsEnabled{true};

		EBodyType mBodyType = EBodyType::Static;

		float mMass = 1.0f;

		float mAngularDamping = 0.0f;

		float mLinearDamping = 0.0f;

		TEnumAsByte<ELockAxis> mLinearLockAxis = NoAxis;

		TEnumAsByte<ELockAxis> mAngularLockAxis = NoAxis;

		bool mGravityEnabled = true;

		void ApplyForce(const glm::vec3 &force);

		void SetVelocity(const glm::vec3 &velocity);

		glm::vec3 GetVelocity() const;

		void SetRigidBody(void *rigidbody);

		void *GetRigidBody() const { return mRigidBodyInstance; }

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mPhysicsEnabled, mBodyType, mMass, mAngularDamping, mLinearDamping, mLinearLockAxis,
			   mAngularLockAxis, mGravityEnabled);
		}

		REFLECTABLEV()

	private:
		void *mRigidBodyInstance = nullptr;
	};

	REFLECT(IPhysicsComponent)
	{
		BEGIN_REFLECT(IPhysicsComponent)
		REFLECT_PROPERTY("Physics Enabled", mPhysicsEnabled)
		REFLECT_PROPERTY("BodyType", mBodyType)
		REFLECT_PROPERTY("Mass", mMass)
		REFLECT_PROPERTY("Angular Damping", mAngularDamping)
		REFLECT_PROPERTY("Linear Damping", mLinearDamping)
		REFLECT_PROPERTY("Linear Lock Axis", mLinearLockAxis)
		REFLECT_PROPERTY("Angular Lock Axis", mAngularLockAxis)
		REFLECT_PROPERTY("Gravity Enabled", mGravityEnabled);
	}

	REFLECT(EBodyType)
	{
		BEGIN_REFLECT_ENUM(EBodyType)
		(ENUM_VALUE(Static), ENUM_VALUE(Kinematic), ENUM_VALUE(Dynamic));
	}

} // namespace BHive