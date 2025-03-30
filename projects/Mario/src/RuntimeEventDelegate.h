#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	struct GameObject;

	REFLECT_STRUCT()
	struct RuntimeEventDelegate
	{
		RuntimeEventDelegate() = default;
		RuntimeEventDelegate(const RuntimeEventDelegate &);

		void SetCaller(GameObject *caller);
		GameObject *GetCaller() const { return Object; }

		RuntimeEventDelegate &operator=(const RuntimeEventDelegate &rhs);

	private:
		void OnObjectDestroyed(GameObject *);

		REFLECT_PROPERTY(Setter = SetCaller, Getter = GetCaller)
		GameObject *Object = nullptr;
	};

} // namespace BHive
