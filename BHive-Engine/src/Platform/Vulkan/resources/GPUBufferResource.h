#pragma once

#include "Platform/Vulkan/VulkanCore.h"
#include "Platform/Vulkan/VulkanMemory.h"

namespace BHive
{
	struct BHIVE_API GPUBufferResource
	{
		GPUBufferResource(const GPUBufferResource &) = delete;
		GPUBufferResource &operator=(const GPUBufferResource &) = delete;

		GPUBufferResource(const std::string &name, vk::BufferCreateInfo info, vk::MemoryPropertyFlags flags, size_t Size, MemoryAllocator *allocator);

		~GPUBufferResource();

		void *map(vk::DeviceSize offset = 0, vk::DeviceSize size = VK_WHOLE_SIZE);

		void unmap();

		vk::raii::Buffer Buffer = VK_NULL_HANDLE;

		MemoryAllocation Allocation;

		vk::DeviceSize Size = 0;

	private:
		MemoryAllocator *mAllocator = nullptr;
	};
} // namespace BHive