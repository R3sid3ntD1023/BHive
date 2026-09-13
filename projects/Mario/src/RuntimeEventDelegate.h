#pragma once

#include "Core.h"
#include "core/reflection/Reflection.h"
#include "runtime/GameObject.h"

namespace BHive
{
	struct GameObject;

	struct MARIO_API RuntimeEventDelegate
	{
		RuntimeEventDelegate() = default;
		RuntimeEventDelegate(const RuntimeEventDelegate &);

		void SetCaller(GameObject *caller);

		GameObject *GetCaller() const { return Object; }

		RuntimeEventDelegate &operator=(const RuntimeEventDelegate &rhs);

	private:
		void OnObjectDestroyed(GameObject *);

		GameObject *Object = nullptr;
	};

} // namespace BHive
