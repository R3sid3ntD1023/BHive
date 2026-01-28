#pragma once

#include <core/Core.h>

namespace BHive
{
	class BHIVE_API PhysicsContext
	{
	public:
		void Init();
		void Shutdown();
		void *GetPhysics() const;

	private:
		void *mFoundation = nullptr;
		void *mPhysics = nullptr;
		void *mPvd = nullptr;
		void *mAllocator = nullptr;
		void *mErrorCallabck = nullptr;
	};
} // namespace BHive