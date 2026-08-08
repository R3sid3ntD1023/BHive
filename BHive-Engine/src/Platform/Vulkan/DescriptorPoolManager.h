#pragma once

#include "VulkanCore.h"

namespace BHive
{

	struct DescriptorPoolManager
	{
		vk::DescriptorPool GetPool() const;

		void Init(vk::raii::Device &device);

		void Shutdown();

	private:
		vk::raii::DescriptorPool mPool = VK_NULL_HANDLE;
	};

} // namespace BHive