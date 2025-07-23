#pragma once

#include "PhysicsCore.h"

namespace physx
{
	class PxDefaultErrorCallback;
	class PxDefaultAllocator;
	class PxDefaultCpuDispatcher;
	class PxToleranceScale;

	class PxFoundation;
	class PxPhysics;
	class PxPvd;
} // namespace physx

namespace BHive
{
	class PhysicsLogger;

	class BHIVE_API PhysicsContext
	{
	public:
		static void Init();
		static void Shutdown();
		static physx::PxPhysics *GetPhysics() { return sPhysics; }

	private:
		static inline physx::PxFoundation *sFoundation = nullptr;
		static inline physx::PxPhysics *sPhysics = nullptr;
		static inline physx::PxPvd *sPvd = nullptr;
		static inline physx::PxDefaultAllocator *sAllocator = nullptr;
		static inline physx::PxDefaultErrorCallback *sErrorCallabck = nullptr;
	};
} // namespace BHive