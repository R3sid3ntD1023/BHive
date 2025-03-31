#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	struct GameObject;

	DECLARE_STRUCT()
	struct RuntimeEventDelegate
	{
		RuntimeEventDelegate() = default;
		RuntimeEventDelegate(const RuntimeEventDelegate &);

		DECLARE_FUNCTION()
		void SetCaller(GameObject *caller);

		DECLARE_FUNCTION()
		GameObject *GetCaller() const { return Object; }

		RuntimeEventDelegate &operator=(const RuntimeEventDelegate &rhs);

	private:
		void OnObjectDestroyed(GameObject *);

		DECLARE_PROPERTY(Setter = SetCaller, Getter = GetCaller)
		GameObject *Object = nullptr;


	};

} // namespace BHive
