#include "EventListener.h"

namespace BHive
{
	void SimulationCallback::onContact(const PxContactPairHeader &pairheader, const PxContactPair *pairs, PxU32 nbPairs)
	{

	}

	void SimulationCallback::onTrigger(PxTriggerPair *pairs, PxU32 count)
	{
	}
	void SimulationCallback::onConstraintBreak(PxConstraintInfo *constraints, PxU32 count)
	{
	}
	void SimulationCallback::onWake(PxActor **actors, PxU32 count)
	{
	}
	void SimulationCallback::onSleep(PxActor **actors, PxU32 count)
	{
	}
	void
	SimulationCallback::onAdvance(const PxRigidBody *const *bodyBuffer, const PxTransform *poseBuffer, const PxU32 count)
	{
	}

} // namespace BHive