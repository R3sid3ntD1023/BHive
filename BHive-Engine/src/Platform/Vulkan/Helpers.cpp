#include "Helpers.h"
#include "VulkanCore.h"

namespace BHive
{
	AllocatedVulkanBuffer::AllocatedVulkanBuffer(AllocatedVulkanBuffer &&other) noexcept
		: Memory(std::move(other.Memory)),
		  Buffer(std::move(other.Buffer)),
		  Size(other.Size),
		  MappedMemory(other.MappedMemory)
	{
		other.Buffer = VK_NULL_HANDLE;
		other.Memory = VK_NULL_HANDLE;
		other.MappedMemory = nullptr;
		other.Size = 0;
	}

	AllocatedVulkanBuffer::~AllocatedVulkanBuffer()
	{
		if (MappedMemory)
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