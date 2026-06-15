#include "MemoryAllocator.h"
#include "VulkanBackend.h"

namespace BHive
{
	MemoryAllocator::MemoryAllocator(vk::Device device, vk::PhysicalDevice physicalDevice)
		: mDevice(device),
		  mPhysicalDevice(physicalDevice)
	{
	}

	MemoryAllocation MemoryAllocator::Allocate(const vk::raii::Buffer &buffer, vk::MemoryPropertyFlags props, size_t requestedBufferSize)
	{
		vk::MemoryRequirements req = buffer.getMemoryRequirements();
		auto memoryTypeIndex = FindMemoryType(req.memoryTypeBits, props);
		if (ShouldUseDedicatedAllocation(requestedBufferSize))
		{
			return AllocateDedicated(req, memoryTypeIndex);
		}

		return AllocateFromBlock(req, memoryTypeIndex);
	}

	MemoryAllocation MemoryAllocator::Allocate(const vk::raii::Image &image, vk::MemoryPropertyFlags props, size_t requestedBufferSize)
	{
		vk::MemoryRequirements req = image.getMemoryRequirements();
		auto memoryTypeIndex = FindMemoryType(req.memoryTypeBits, props);
		if (ShouldUseDedicatedAllocation(requestedBufferSize))
		{
			return AllocateDedicated(req, memoryTypeIndex);
		}

		return AllocateFromBlock(req, memoryTypeIndex);
	}

	void* MemoryAllocator::Map(MemoryAllocation &allocation)
	{
		if (allocation.IsDedicated)
		{
			if (!allocation.IsMapped)
			{
				void *ptr = mDevice.mapMemory(allocation.Memory, allocation.Offset, allocation.Size);
				allocation.IsMapped = true;
				allocation.MappedPtr = ptr;
			}
			
			return allocation.MappedPtr;
		}
		
		Block *block = static_cast<Block *>(allocation.BlockUserData);
		if (!block)
			return nullptr;

		if (!block->MappedPtr)
			block->MappedPtr = mDevice.mapMemory(block->Memory, 0, block->Size);

		allocation.MappedPtr = static_cast<char *>(block->MappedPtr) + allocation.Offset;
		allocation.IsMapped = true;

		return allocation.MappedPtr;
	}

	void MemoryAllocator::UnMap(const MemoryAllocation &allocation)
	{
		if (allocation.IsDedicated && allocation.IsMapped)
		{
			mDevice.unmapMemory(allocation.Memory);
		
		}
	}

	void MemoryAllocator::Free(const MemoryAllocation &allocation)
	{
		if (allocation.IsDedicated)
		{
			if (allocation.IsMapped)
				mDevice.unmapMemory(allocation.Memory);

			mDevice.freeMemory(allocation.Memory);
		}
		else
		{
			Block *block = static_cast<Block *>(allocation.BlockUserData);
			if (!block)
				return;

			block->FreeList.emplace_back(allocation.Offset, allocation.Size);

			MergeFreeList(block);

			if (block->FreeList.size() == 1 && block->FreeList[0] == std::pair{0, block->Size})
			{
				if (block->MappedPtr)
				{
					mDevice.unmapMemory(block->Memory);
					block->MappedPtr = nullptr;
				}
			}
		}
	}

	uint32_t MemoryAllocator::FindMemoryType(uint32_t memoryTypeIndex, vk::MemoryPropertyFlags properties)
	{
		auto memoryProperties = mPhysicalDevice.getMemoryProperties();
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((memoryTypeIndex & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		ASSERT(false, "Failed to find suitable memory type!")
		return ~0u;
	}

	bool MemoryAllocator::ShouldUseDedicatedAllocation(size_t requestedBufferSize) const
	{
		constexpr vk::DeviceSize DEDICATED_ALLOCATION_THRESHOLD = 1ull << 20; // 1 MB
		return (vk::DeviceSize)requestedBufferSize >= DEDICATED_ALLOCATION_THRESHOLD;
	}

	MemoryAllocation MemoryAllocator::AllocateDedicated(const vk::MemoryRequirements &req, uint32_t memoryTypeIndex)
	{
		vk::MemoryAllocateInfo allocInfo(req.size, memoryTypeIndex);
		vk::DeviceMemory memory = mDevice.allocateMemory(allocInfo);

		MemoryAllocation allocation;
		allocation.Memory = memory;
		allocation.Offset = 0;
		allocation.Size = req.size;
		allocation.IsDedicated = true;
		allocation.MemoryTypeIndex = memoryTypeIndex;
		return allocation;
	}

	MemoryAllocation MemoryAllocator::AllocateFromBlock(const vk::MemoryRequirements &req, uint32_t memoryTypeIndex)
	{
		auto &blocks = mBlocksPerType[memoryTypeIndex];

		// try existing blocks
		for (auto& block : blocks)
		{
			auto alloc = AllocateFromBlock(block, req, memoryTypeIndex);
			if (alloc.Size != 0)
				return alloc;
		}

		// create new block
		vk::DeviceSize blockSize = std::max<vk::DeviceSize>(req.size * 4, 32ull << 20);
		Block &newBlock = CreateBlock(memoryTypeIndex, blockSize);
		return AllocateFromBlock(newBlock, req, memoryTypeIndex);
	}

	MemoryAllocation MemoryAllocator::AllocateFromBlock(Block &block, const vk::MemoryRequirements &req, uint32_t memoryTypeIndex)
	{
		for (size_t i = 0; i < block.FreeList.size(); i++)
		{
			auto &[offset, size] = block.FreeList[i];
			vk::DeviceSize alignedOffset = (offset + req.alignment - 1) & ~(req.alignment - 1);

			vk::DeviceSize padding = alignedOffset - offset;
			if (padding + req.size > size)
				continue;

			vk::DeviceSize remainingSize = size - (padding + req.size);
			block.FreeList.erase(block.FreeList.begin() + i);

			if (padding > 0)
				block.FreeList.emplace_back(offset, padding);

			if (remainingSize > 0)
				block.FreeList.emplace_back(alignedOffset + req.size, remainingSize);

			MemoryAllocation allocation{};
			allocation.Memory = block.Memory;
			allocation.Offset = alignedOffset;
			allocation.Size = req.size;
			allocation.IsDedicated = false;
			allocation.MemoryTypeIndex = memoryTypeIndex;
			allocation.BlockUserData = &block;
			allocation.MappedPtr = block.MappedPtr ? static_cast<char *>(block.MappedPtr) + alignedOffset : nullptr;
			allocation.IsMapped = block.MappedPtr != nullptr;
			return allocation;
		}

		return {};
	}

	MemoryAllocator::Block &MemoryAllocator::CreateBlock(uint32_t memoryTypeIndex, vk::DeviceSize size)
	{
		vk::MemoryAllocateInfo allocInfo(size, memoryTypeIndex);
		vk::DeviceMemory memory = mDevice.allocateMemory(allocInfo);

		Block newBlock{};
		newBlock.Memory = memory;
		newBlock.Size = size;
		newBlock.FreeList.emplace_back(0, size);

		mBlocksPerType[memoryTypeIndex].push_back(std::move(newBlock));
		return mBlocksPerType[memoryTypeIndex].back();
	}

	void MemoryAllocator::MergeFreeList(Block *block)
	{
		if (!block)
			return;

		auto freeList = block->FreeList;
		std::sort(block->FreeList.begin(), block->FreeList.end(), [](auto& a, auto&b){ return a.first < b.first;
			});

		FFreeList merged;
		merged.reserve(block->FreeList.size());

		for (auto &[offset, size] : freeList)
		{
			if (merged.empty())
			{
				merged.emplace_back(offset, size);
				continue;

			}

			
			auto &[lastOffset, lastSize] = merged.back();
			if (lastOffset + lastSize == offset)
			{
				lastSize += size;
			}
			else
			{
				merged.emplace_back(offset, size);
			}
		}

		freeList = std::move(merged);
	}
} // namespace BHive