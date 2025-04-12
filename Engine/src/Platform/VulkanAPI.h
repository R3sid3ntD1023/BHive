#pragma once

#include "core/Core.h"
#include "VulkanInstance.h"

namespace BHive
{
	class VulkanAPI
	{
	public:
		static void Init();
		static void Shutdown();
		static VulkanInstance &GetInstance() { return *mInstance; }

	private:
		static Scope<VulkanInstance> mInstance;
	};
} // namespace BHive