#pragma once

#include "core/Core.h"
#include "core/EnumAsByte.h"
#include "physics/LockAxis.h"
#include "world/Component.h"
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

		float LinearDamping = 0.0f;

		float AngularDamping = 0.0f;

		TEnumAsByte<ELockAxis> LinearLockAxis = NoAxis;

		TEnumAsByte<ELockAxis> AngularLockAxis = NoAxis;

		bool GravityEnabled = true;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(PhysicsEnabled, BodyType, Mass, LinearDamping, AngularDamping, LinearLockAxis, AngularLockAxis,
			   GravityEnabled);
		}
	};

	struct PhysicsComponent : public Component
	{
		PhysicsComponent() = default;
		PhysicsComponent(const PhysicsComponent &) = default;

		PhysicsSettings Settings;

		void Update(float) override;

		void ApplyForce(const glm::vec3 &force);

		void SetBodyType(EBodyType type);

		void SetVelocity(const glm::vec3 &velocity);

		glm::vec3 GetVelocity() const;

		void SetRigidBody(void *rigidbody);

		void *GetRigidBody() { return mRigidBodyInstance; }

		EBodyType GetBodyType() const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Component)

	private:
		void *mRigidBodyInstance = nullptr;
	};

	REFLECT_EXTERN(PhysicsComponent)
} // namespace BHive