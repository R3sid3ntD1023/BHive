#include "PhysicsContext.h"
#include "PhysicsLogger.h"

namespace BHive
{
	void PhysicsContext::Init()
	{
		sLogger = new PhysicsLogger();
		sPhyicsCommon.setLogger(sLogger);
	}

	void PhysicsContext::Shutdown()
	{
		delete sLogger;
	}
}
