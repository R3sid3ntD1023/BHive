#pragma once

#pragma warning(push, 0)
#include <physx/PxPhysicsAPI.h>
#pragma warning(pop)

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

} // namespace BHive
