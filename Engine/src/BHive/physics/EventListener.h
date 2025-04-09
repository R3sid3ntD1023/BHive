#pragma once

#include "core/EventDelegate.h"
#include "math/Math.h"
#include "PhysicsCore.h"

namespace BHive
{

	using namespace physx;

	class SimulationCallback : public PxSimulationEventCallback
	{
	public:
		virtual void onContact(const PxContactPairHeader &pairheader, const PxContactPair *pairs, PxU32 nbPairs);

		virtual void onTrigger(PxTriggerPair *pairs, PxU32 count);

		virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count);

		virtual void onWake(PxActor **actors, PxU32 count);

		virtual void onSleep(PxActor **actors, PxU32 count);

		virtual void onAdvance(const PxRigidBody *const *bodyBuffer, const PxTransform *poseBuffer, const PxU32 count);
	};

	struct FHitResult
	{
		bool InitalOverlap{};

		glm::vec3 Normal{};
		glm::vec3 Position{};
		float Distance{};

		struct GameObject *Object = nullptr;
		struct Component *Component = nullptr;

		bool isValid() const { return Object && Component; }
	};

} // namespace BHive
