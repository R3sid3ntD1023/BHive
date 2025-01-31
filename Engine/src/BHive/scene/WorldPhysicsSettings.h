#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "math/Math.h"

namespace BHive
{
	struct FWorldPhysicsSettings
	{
		// Name of the world
		std::string worldName;

		/// Gravity force vector of the world (in Newtons)
		glm::vec3 gravity;

		/// Distance threshold for two contact points for a valid persistent contact (in meters)
		float persistentContactDistanceThreshold;

		/// Default friction coefficient for a rigid body
		float defaultFrictionCoefficient;

		/// Default bounciness factor for a rigid body
		float defaultBounciness;

		/// Velocity threshold for contact velocity restitution
		float restitutionVelocityThreshold;

		/// True if the sleeping technique is enabled
		bool isSleepingEnabled;

		/// Number of iterations when solving the velocity constraints of the Sequential Impulse technique
		uint16_t defaultVelocitySolverNbIterations;

		/// Number of iterations when solving the position constraints of the Sequential Impulse technique
		uint16_t defaultPositionSolverNbIterations;

		/// Time (in seconds) that a body must stay still to be considered sleeping
		float defaultTimeBeforeSleep;

		/// A body with a linear velocity smaller than the sleep linear velocity (in m/s)
		/// might enter sleeping mode.
		float defaultSleepLinearVelocity;

		/// A body with angular velocity smaller than the sleep angular velocity (in rad/s)
		/// might enter sleeping mode
		float defaultSleepAngularVelocity;

		/// This is used to test if two contact manifold are similar (same contact normal) in order to
		/// merge them. If the cosine of the angle between the normals of the two manifold are larger
		/// than the value bellow, the manifold are considered to be similar.
		float cosAngleSimilarContactManifold;

		FWorldPhysicsSettings()
		{

			worldName = "";
			gravity = glm::vec3(0, -9.81f, 0);
			persistentContactDistanceThreshold = 0.03f;
			defaultFrictionCoefficient = 0.3f;
			defaultBounciness = 0.5f;
			restitutionVelocityThreshold = 0.5f;
			isSleepingEnabled = true;
			defaultVelocitySolverNbIterations = 6;
			defaultPositionSolverNbIterations = 3;
			defaultTimeBeforeSleep = 1.0f;
			defaultSleepLinearVelocity = 0.02f;
			defaultSleepAngularVelocity = 3.0f * (PI / 180.0f);
			cosAngleSimilarContactManifold = 0.95f;
		}

		template <typename A>
		void Serialize(A &ar)
		{
			ar(worldName, gravity, persistentContactDistanceThreshold, defaultFrictionCoefficient, defaultBounciness, restitutionVelocityThreshold,
			   isSleepingEnabled, defaultVelocitySolverNbIterations, defaultPositionSolverNbIterations, defaultTimeBeforeSleep,
			   defaultSleepLinearVelocity, defaultSleepAngularVelocity, cosAngleSimilarContactManifold);
		}

		operator rp3d::PhysicsWorld::WorldSettings() const
		{
			rp3d::PhysicsWorld::WorldSettings settings{};
			settings.worldName = worldName;
			settings.gravity = rp3d::Vector3(gravity.x, gravity.y, gravity.z);
			settings.persistentContactDistanceThreshold = persistentContactDistanceThreshold;
			settings.defaultFrictionCoefficient = defaultFrictionCoefficient;
			settings.defaultBounciness = defaultBounciness;
			settings.restitutionVelocityThreshold = restitutionVelocityThreshold;
			settings.isSleepingEnabled = isSleepingEnabled;
			settings.defaultVelocitySolverNbIterations = defaultVelocitySolverNbIterations;
			settings.defaultPositionSolverNbIterations = defaultPositionSolverNbIterations;
			settings.defaultTimeBeforeSleep = defaultTimeBeforeSleep;
			settings.defaultSleepLinearVelocity = defaultSleepLinearVelocity;
			settings.defaultSleepAngularVelocity = defaultSleepAngularVelocity;
			settings.cosAngleSimilarContactManifold = cosAngleSimilarContactManifold;
			return settings;
		}

		REFLECTABLE()
	};

	REFLECT(FWorldPhysicsSettings)
	{
		BEGIN_REFLECT(FWorldPhysicsSettings)
		REFLECT_PROPERTY("Gravity", gravity)
		(META_DATA(EPropertyMetaData_Default, glm::vec3(0 COMMA - 9.81f COMMA 0)))
			REFLECT_PROPERTY("Persistent Contact Distance Threshold", persistentContactDistanceThreshold)(META_DATA(EPropertyMetaData_Default, 0.03f))
				REFLECT_PROPERTY("Default Friction Coefficient", defaultFrictionCoefficient)(META_DATA(EPropertyMetaData_Default, 0.3f))
					REFLECT_PROPERTY("Default Bounciness", defaultBounciness)(META_DATA(EPropertyMetaData_Default, 0.5f))
						REFLECT_PROPERTY("Restitution Velocity Threshold", restitutionVelocityThreshold)(META_DATA(EPropertyMetaData_Default, 0.5f))
							REFLECT_PROPERTY("Is Sleeping Enabled", isSleepingEnabled)(META_DATA(EPropertyMetaData_Default, true)) REFLECT_PROPERTY(
								"Default Velocity Solver NbIterations", defaultVelocitySolverNbIterations)(META_DATA(EPropertyMetaData_Default, 6))
								REFLECT_PROPERTY("Default Position Solver NbIterations", defaultPositionSolverNbIterations)(
									META_DATA(EPropertyMetaData_Default, 3))
									REFLECT_PROPERTY("Default Time Before Sleep", defaultTimeBeforeSleep)(META_DATA(EPropertyMetaData_Default, 1.f))
										REFLECT_PROPERTY("Default Sleep Linear Velocity", defaultSleepLinearVelocity)(
											META_DATA(EPropertyMetaData_Default, 0.02f))
											REFLECT_PROPERTY("Default Sleep Angular Velocity", defaultSleepAngularVelocity)(
												META_DATA(EPropertyMetaData_Default, 3.0f * (PI / 180.0f)))
												REFLECT_PROPERTY("CosAngle Similar Contact Manifold", cosAngleSimilarContactManifold)(
													META_DATA(EPropertyMetaData_Default, 0.95f));
	}
} // namespace BHive