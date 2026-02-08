#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace BHive
{
	struct AllocatedVulkanTexture
	{
		vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
		vk::raii::Image Image = VK_NULL_HANDLE;
		vk::raii::ImageView ImageView = VK_NULL_HANDLE;
		vk::raii::Sampler Sampler = VK_NULL_HANDLE;
	};

	struct AllocatedVulkanBuffer
	{
		vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
		vk::raii::Buffer Buffer = VK_NULL_HANDLE;
	};

	
} // namespace BHive