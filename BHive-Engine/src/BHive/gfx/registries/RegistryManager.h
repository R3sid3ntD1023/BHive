#pragma once

#include "core/Core.h"
#include "core/subsystem/SubSystem.h"
#include "core/type/TypeID.h"

namespace BHive
{
	class IResourceRegistry;

	template <typename T>
	class ResourceRegistry;

	struct ResourceRegistriesManager
	{
		static void Init();

		static void Shutdown();

		template <typename T>
		static void RegisterRegistry()
		{
			using U = ResourceRegistry<T>;

			auto type = TypeID<T>::value;
			Registries[type] = &AddSubSystem<U>();
		}

		template <typename T>
		static void UnRegisterRegistry()
		{
			using U = ResourceRegistry<T>;

			auto type = TypeID<T>::value;
			Registries.erase(type);
			RemoveSubSystem<U>();
		}

		static IResourceRegistry *GetRegistry(uint32_t type);

	private:
		static inline std::unordered_map<uint32_t, IResourceRegistry *> Registries;
	};
} // namespace BHive