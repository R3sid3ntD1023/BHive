#include "PhysicsContext.h"
#include <physx/PxPhysicsAPI.h>

namespace BHive
{
	class PhysxErrorCallback : public physx::PxDefaultErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line)
		{
			LOG_ERROR("Physx::ERROR : {}", message);
		}
	};

	void PhysicsContext::Init()
	{
		sAllocator = new physx::PxDefaultAllocator;
		sErrorCallabck = new physx::PxDefaultErrorCallback;

		physx::PxTolerancesScale sTolerancesScale;

		sFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *sAllocator, *sErrorCallabck);
		if (!sFoundation)
			ASSERT(false, "PxCreateFundation Failed ");

#ifdef _DEBUG
		sPvd = PxCreatePvd(*sFoundation);
		physx::PxPvdTransport *transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		sPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif // DEBUG

		sTolerancesScale.length = 100;
		sTolerancesScale.speed = 981;
		sPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *sFoundation, sTolerancesScale, true, sPvd);
	}

	void PhysicsContext::Shutdown()
	{

		sPhysics->release();
		sFoundation->release();

		delete sErrorCallabck;
		delete sAllocator;
	}
} // namespace BHive
