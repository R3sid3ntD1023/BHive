#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	struct GameObject;

	struct RuntimeEventDelegate
	{
		RuntimeEventDelegate() = default;
		RuntimeEventDelegate(const RuntimeEventDelegate &);

		void SetCaller(GameObject *caller);
		GameObject *GetCaller() const { return Object; }

		RuntimeEventDelegate &operator=(const RuntimeEventDelegate &rhs);

	private:
		void OnObjectDestroyed(GameObject *);

		GameObject *Object = nullptr;

		REFLECTABLE()
	};

} // namespace BHive
