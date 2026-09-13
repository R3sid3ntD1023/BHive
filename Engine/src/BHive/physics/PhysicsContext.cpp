#include "PhysicsContext.h"
#include <physx/PxPhysicsAPI.h>
#define WINDOWS_MEAN_AND_LEAN
#include <Windows.h>

#define PVD_HOST "127.0.0.1"

namespace BHive
{

	class PhysxErrorCallback : public physx::PxDefaultErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line) { LOG_ERROR("Physx::ERROR : {}", message); }
	};

	static physx::PxDefaultAllocator *sAllocator = nullptr;
	static PhysxErrorCallback *sErrorCallback = nullptr;

	void PhysicsContext::Init()
	{
		HMODULE common = LoadLibraryA("PhysXCommon_64.dll");
		if (!common)
		{
			DWORD error = GetLastError();
			LOG_ERROR("Failed to load PhysxCommon.dll! : {}", error);
		}

		bool recordMemoryAllocations = true;

		LOG_INFO("Physx: verison {}", PX_PHYSICS_VERSION);

		sAllocator = new physx::PxDefaultAllocator();
		sErrorCallback = new PhysxErrorCallback();

		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *sAllocator, *sErrorCallback);

		ASSERT(mFoundation, "PxCreateFundation Failed ");

		mPvd = PxCreatePvd(*(physx::PxFoundation *)mFoundation);

		physx::PxPvdTransport *transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		((physx::PxPvd *)mPvd)->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *(physx::PxFoundation *)mFoundation, physx::PxTolerancesScale(100, 981), recordMemoryAllocations, (physx::PxPvd *)mPvd);
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

		delete sErrorCallback;
		delete sAllocator;
	}
} // namespace BHive
