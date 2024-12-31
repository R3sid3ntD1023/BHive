#pragma once

#include "PhysicsCore.h"

namespace BHive
{
	class PhysicsLogger;

	class PhysicsContext
	{
	public:
		static BHIVE void Init();
		static BHIVE void Shutdown();
		static BHIVE rp3d::PhysicsCommon& get_context() { return sPhyicsCommon; }

	private:
		static inline rp3d::PhysicsCommon sPhyicsCommon;
		static inline PhysicsLogger* sLogger;
	};
}