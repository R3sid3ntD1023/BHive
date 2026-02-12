#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace BHive
{
	namespace Vulkan
	{
		struct AllocatedTexture
		{
			vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
			vk::raii::Image Image = VK_NULL_HANDLE;
			vk::raii::ImageView ImageView = VK_NULL_HANDLE;
			vk::raii::Sampler Sampler = VK_NULL_HANDLE;
		};

		struct AllocatedBuffer
		{
			vk::raii::Buffer Buffer = VK_NULL_HANDLE;
			vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
		};

	}
	
} // namespace BHive