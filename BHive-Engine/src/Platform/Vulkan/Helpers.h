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
		AllocatedVulkanBuffer();

		~AllocatedVulkanBuffer();

		vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
		vk::raii::Buffer Buffer = VK_NULL_HANDLE;
	
		void SetData(const void *data, size_t size, uint32_t offset = 0);

	private:
		
		vk::raii::Device & mDevice;
		vk::DeviceSize Size = 0;
		void *MappedMemory = nullptr;

		friend class VulkanUtils;
	};

	
} // namespace BHive