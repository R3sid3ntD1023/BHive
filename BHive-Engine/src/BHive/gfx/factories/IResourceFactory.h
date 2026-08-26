#pragma once

#include "core/Core.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/registries/ResourceRegistry.h"

namespace BHive
{

	template <typename T>
	struct IResourceFactory
	{
		static ResourceRegistry<T> *GetResourceRegsitry() { return dynamic_cast<ResourceRegistry<T> *>(Renderer::Get().GetResourceRegistry<T>()); }
	};
} // namespace BHive