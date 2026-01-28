#include "PhysicsContext.h"
#include <physx/PxPhysicsAPI.h>

namespace BHive
{
	class PhysxErrorCallback : public physx::PxDefaultErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line) { LOG_ERROR("Physx::ERROR : {}", message); }
	};

	void PhysicsContext::Init()
	{
		mAllocator = new physx::PxDefaultAllocator;
		mErrorCallabck = new physx::PxDefaultErrorCallback;

		physx::PxTolerancesScale sTolerancesScale;

		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *(physx::PxDefaultAllocator *)mAllocator, *(physx::PxDefaultErrorCallback *)mErrorCallabck);
		if (!mFoundation)
			ASSERT(false, "PxCreateFundation Failed ");

#ifdef _DEBUG

		mPvd = PxCreatePvd(*(physx::PxFoundation *)mFoundation);
		physx::PxPvdTransport *transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		((physx::PxPvd *)mPvd)->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif // DEBUG

		sTolerancesScale.length = 100;
		sTolerancesScale.speed = 981;
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *(physx::PxFoundation *)mFoundation, sTolerancesScale, true, (physx::PxPvd *)mPvd);
	}

	void *PhysicsContext::GetPhysics() const
	{
		ASSERT(mPhysics);
		return mPhysics;
	}

	void PhysicsContext::Shutdown()
	{

		((physx::PxPhysics *)mPhysics)->release();
		((physx::PxFoundation *)mFoundation)->release();

		delete mErrorCallabck;
		delete mAllocator;
	}
} // namespace BHive
