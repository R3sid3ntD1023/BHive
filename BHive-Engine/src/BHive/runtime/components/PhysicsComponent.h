#pragma once

#include "core/Core.h"
#include "core/EnumAsByte.h"
#include "core/math/Math.h"
#include "physics/LockAxis.h"
#include "runtime/Component.h"

namespace BHive
{

	enum class EBodyType : uint8_t
	{
		Static,
		Kinematic,
		Dynamic
	};

	struct BHIVE_API PhysicsSettings
	{
		bool PhysicsEnabled{true};

		EBodyType BodyType = EBodyType::Static;

		TEnumAsByte<ELockAxis> LinearLockAxis = NoAxis;

		TEnumAsByte<ELockAxis> AngularLockAxis = NoAxis;

		float Mass = 1.0f;

		float LinearDamping = 0.0f;

		float AngularDamping = 0.0f;

		bool GravityEnabled = true;

		glm::vec3 InitialVelocity = {0.f, 0.f, 0.f};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(PhysicsEnabled, BodyType, Mass, LinearDamping, AngularDamping, GravityEnabled, InitialVelocity, LinearLockAxis, AngularLockAxis);
		}
	};

	struct BHIVE_API PhysicsComponent : public Component, public ITickable
	{
		PhysicsComponent() = default;
		PhysicsComponent(const PhysicsComponent &) = default;

		PhysicsSettings Settings;

		void Update(float) override;

		void ApplyForce(const glm::vec3 &force);

		void SetBodyType(EBodyType type);

		void SetVelocity(const glm::vec3 &velocity);

		void SetGravityEnabled(bool enabled);

		glm::vec3 GetVelocity() const;

		void SetRigidBody(void *rigidbody);

		void *GetRigidBody() { return mRigidBodyInstance; }

		EBodyType GetBodyType() const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Component, ITickable)

	private:
		void *mRigidBodyInstance = nullptr;
	};

	REFLECT_EXTERN(PhysicsComponent)
} // namespace BHive