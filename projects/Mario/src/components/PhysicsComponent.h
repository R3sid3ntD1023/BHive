#pragma once

#include "Component.h"
#include "core/Core.h"
#include "core/EnumAsByte.h"
#include "physics/LockAxis.h"
#include <glm/glm.hpp>

namespace BHive
{
	DECLARE_ENUM()
	enum class EBodyType : int
	{
		Static,
		Kinematic,
		Dynamic
	};

	DECLARE_STRUCT()
	struct PhysicsSettings
	{
		DECLARE_PROPERTY()
		bool PhysicsEnabled = true;

		DECLARE_PROPERTY()
		EBodyType BodyType = EBodyType::Static;

		DECLARE_PROPERTY()
		float Mass = 1.0f;

		DECLARE_PROPERTY()
		float LinearDamping = 0.0f;

		DECLARE_PROPERTY()
		float AngularDamping = 0.0f;

		DECLARE_PROPERTY()
		TEnumAsByte<ELockAxis> LinearLockAxis = NoAxis;

		DECLARE_PROPERTY()
		TEnumAsByte<ELockAxis> AngularLockAxis = NoAxis;

		DECLARE_PROPERTY()
		bool GravityEnabled = true;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(PhysicsEnabled, BodyType, Mass, LinearDamping, AngularDamping, LinearLockAxis, AngularLockAxis,
			   GravityEnabled);
		}
	};

	DECLARE_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct PhysicsComponent : public Component
	{
		DECLARE_CONSTRUCTOR()
		PhysicsComponent() = default;

		PhysicsComponent(const PhysicsComponent &) = default;

		DECLARE_PROPERTY()
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

		REFLECTABLEV(Component)

	private:
		void *mRigidBodyInstance = nullptr;
	};
} // namespace BHive