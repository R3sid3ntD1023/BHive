#pragma once

#include "Component.h"
#include "core/Core.h"
#include "core/EnumAsByte.h"
#include "physics/LockAxis.h"
#include <glm/glm.hpp>

namespace BHive
{
	REFLECT_ENUM()
	enum class EBodyType : int
	{
		Static,
		Kinematic,
		Dynamic
	};

	REFLECT_STRUCT()
	struct PhysicsSettings
	{
		REFLECT_PROPERTY()
		bool PhysicsEnabled = true;

		REFLECT_PROPERTY()
		EBodyType BodyType = EBodyType::Static;

		REFLECT_PROPERTY()
		float Mass = 1.0f;

		REFLECT_PROPERTY()
		float LinearDamping = 0.0f;

		REFLECT_PROPERTY()
		float AngularDamping = 0.0f;

		REFLECT_PROPERTY()
		TEnumAsByte<ELockAxis> LinearLockAxis = NoAxis;

		REFLECT_PROPERTY()
		TEnumAsByte<ELockAxis> AngularLockAxis = NoAxis;

		REFLECT_PROPERTY()
		bool GravityEnabled = true;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(PhysicsEnabled, BodyType, Mass, LinearDamping, AngularDamping, LinearLockAxis, AngularLockAxis,
			   GravityEnabled);
		}
	};

	REFLECT_CLASS()
	struct PhysicsComponent : public Component
	{
		REFLECT_CONSTRUCTOR()
		PhysicsComponent() = default;

		PhysicsComponent(const PhysicsComponent &) = default;

		REFLECT_PROPERTY()
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

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)

	private:
		void *mRigidBodyInstance = nullptr;
	};
} // namespace BHive