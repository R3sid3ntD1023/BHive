#pragma once

#include "core/subsystem/SubSystem.h"
#include "ResourceRegistry.h"

namespace BHive
{
	struct ResourceRegistriesManager
	{
		static void Init();

		static void Shutdown();

		template <typename T>
		static void RegisterRegistry()
		{
			using U = ResourceRegistry<T>;

			AddSubSystem<U>();
		}

		template <typename T>
		static void UnRegisterRegistry()
		{
			using U = ResourceRegistry<T>;

			RemoveSubSystem<U>();
		}
	};
} // namespace BHive