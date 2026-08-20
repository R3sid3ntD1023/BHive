#pragma once

#include "VulkanCore.h"

namespace BHive
{
	struct MemoryAllocation
	{
		vk::DeviceMemory Memory = VK_NULL_HANDLE;
		vk::DeviceSize Offset = 0;
		vk::DeviceSize Size = 0;
		bool IsDedicated = false;
		uint32_t MemoryTypeIndex = 0;
		void *BlockUserData = nullptr;
		bool IsMapped = false;
		void *MappedPtr = nullptr;
	};

	class MemoryAllocator
	{
	public:
		MemoryAllocator(vk::Device device, vk::PhysicalDevice physicalDevice);

		MemoryAllocation Allocate(const vk::raii::Buffer &buffer, vk::MemoryPropertyFlags props);

		MemoryAllocation Allocate(const vk::raii::Image &image, vk::MemoryPropertyFlags props);

		void *Map(MemoryAllocation &allocation);

		void UnMap(const MemoryAllocation &allocation);

		void Free(const MemoryAllocation &allocation);

	private:
		vk::Device mDevice = nullptr;
		vk::PhysicalDevice mPhysicalDevice = nullptr;

		using FFreeList = std::vector<std::pair<vk::DeviceSize, vk::DeviceSize>>;

		struct Block
		{
			vk::DeviceMemory Memory;
			vk::DeviceSize Size;
			FFreeList FreeList;
			void *MappedPtr = nullptr;
		};

		std::deque<Block> mBlocksPerType[VK_MAX_MEMORY_TYPES];

		uint32_t FindMemoryType(uint32_t memoryTypeIndex, vk::MemoryPropertyFlags properties);

		bool ShouldUseDedicatedAllocation(const vk::MemoryRequirements &req) const;

		MemoryAllocation AllocateDedicated(const vk::MemoryRequirements &req, uint32_t memoryTypeIndex);

		MemoryAllocation AllocateFromBlock(const vk::MemoryRequirements &req, uint32_t memoryTypeIndex);

		MemoryAllocation AllocateFromBlock(Block &block, const vk::MemoryRequirements &req, uint32_t memoryTypeIndex);

		Block &CreateBlock(uint32_t memoryTypeIndex, vk::DeviceSize size);

		void MergeFreeList(Block *block);

		vk::DeviceSize ChooseBlockSize(vk::DeviceSize req);
	};
} // namespace BHive