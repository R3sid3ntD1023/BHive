#pragma once

#include "core/Core.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/registries/ResourceRegistry.h"

namespace BHive
{

	template <typename T>
	struct IResourceFactory
	{
		static ResourceRegistry<T> *GetResourceRegistry() { return dynamic_cast<ResourceRegistry<T> *>(Renderer::Get().GetResourceRegistry<T>()); }

		template <typename U, typename... Args>
		static ResourceHandle CreateResource(Args &&...args)
		{
			return GetResourceRegistry()->CreateResource<U>(std::forward<Args>(args)...);
		}
	};
} // namespace BHive