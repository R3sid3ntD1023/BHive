#include "Helpers.h"
#include "VulkanCore.h"

namespace BHive
{
	AllocatedVulkanBuffer::AllocatedVulkanBuffer()
		: mDevice(VulkanCore::GetLogicalDevice())
	{
	}

	AllocatedVulkanBuffer::~AllocatedVulkanBuffer()
	{
		Memory.unmapMemory();
	}

	void AllocatedVulkanBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		ASSERT(size <= Size, "Buffer size mismatch!");

		if (!MappedMemory)
			MappedMemory = Memory.mapMemory(offset, size);
		std::memcpy(MappedMemory, data, size);
	}
	
} // namespace BHive