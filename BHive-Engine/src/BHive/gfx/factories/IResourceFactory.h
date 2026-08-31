#pragma once

#include "core/Core.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/registries/ResourceRegistry.h"
#include "gfx/registries/Handles.h"
#include "core/subsystem/SubSystem.h"

namespace BHive
{

	template <typename T>
	struct IResourceFactory
	{
		static ResourceRegistry<T> *GetResourceRegistry()
		{
			using U = ResourceRegistry<T>;

			auto &registry = GetSubSystem<U>();
			return &registry;
		}

		template <typename U, typename... Args>
		static ResourceHandle CreateResource(Args &&...args)
		{
			auto registry = GetResourceRegistry();
			if (!registry)
			{
				LOG_ERROR("IResourceFactory : Registry for {} has not been registered with SubSystem!", TypeName<T>::value);
				return {};
			}
			return registry->CreateResource<U>(std::forward<Args>(args)...);
		}
	};
} // namespace BHive