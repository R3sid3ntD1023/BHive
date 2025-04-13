#pragma once

#include "VulkanCore.h"

namespace BHive
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline();
		~VulkanPipeline();

	private:
		VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
	};

} // namespace BHive