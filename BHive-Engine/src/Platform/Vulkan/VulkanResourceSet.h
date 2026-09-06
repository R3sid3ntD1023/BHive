#pragma once

#include "VulkanBindingGroup.h"
#include "gfx/ResourceSet.h"

namespace BHive
{
	class VulkanResourceSet : public ResourceSet
	{
	public:
		VulkanBindingGroup mGroup;
	};
} // namespace BHive