#pragma once

#include "core/Core.h"

namespace physx
{
	class PxScene;
	class PxDefaultCpuDispatcher;
	class PxSimulationEventCallback;
	template <typename>
	struct PxHitCallback;
	struct PxRaycastHit;
	typedef PxHitCallback<PxRaycastHit> PxRaycastCallback;
} // namespace physx

namespace BHive
{
	class World;
	struct FHitResult;

	struct PhysicsSystem
	{
		void Init(World *world);

		void Update(float dt, World *world);

		void Shutdown(World *world);

		void DebugDraw();

		void InitCallbacks();

		bool RayCast(const glm::vec3 &start, const glm::vec3 &dir, float maxDistance, FHitResult &result, uint16_t categoryMasks = 65535U);

	private:
		physx::PxScene *mScene = nullptr;
		physx::PxDefaultCpuDispatcher *mCpuDispatcher = nullptr;
		physx::PxSimulationEventCallback *mSimulationEventCallback = nullptr;
		physx::PxRaycastCallback *mHitCallback = nullptr;

		float mAccumulatedTime{0.0f};
	};
} // namespace BHive