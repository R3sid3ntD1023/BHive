#include "UniformBuffer.h"
#include "VulkanUtils.h"

namespace BHive
{
	UniformBuffer::UniformBuffer(uint32_t binding, uint64_t size, const void *data)
		: mBinding(binding)
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mBuffer, mMemory);
		if (data)
		{
			SetData(data, size, 0);
		}
	}

	void UniformBuffer::SetData(const void* data, size_t size, uint32_t offset)
	{
		void* buffer_memory = mMemory.mapMemory(offset, size);
		memcpy(buffer_memory, data, size);
		mMemory.unmapMemory();
	}

} // namespace BHive