#include "DescriptorBuilder.h"
#include "UniformBuffer.h"
#include "VulkanUtils.h"

namespace BHive
{
	UniformBuffer::UniformBuffer(uint32_t binding, uint64_t size, const void *data)
		: mBinding(binding)
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mBuffer.Buffer, mBuffer.Memory);
		if (data)
		{
			SetData(data, size, 0);
		}

		mBufferInfo = vk::DescriptorBufferInfo(mBuffer.Buffer, 0, vk::DeviceSize(size));
	}

	void UniformBuffer::SetData(const void* data, size_t size, uint32_t offset)
	{
		void* buffer_memory = mBuffer.Memory.mapMemory(offset, size);
		memcpy(buffer_memory, data, size);
		mBuffer.Memory.unmapMemory();
	}

	void UniformBuffer::WriteDescriptor(const vk::raii::DescriptorSet &set) const
	{
		vk::WriteDescriptorSet write(set, mBinding, 0, vk::DescriptorType::eUniformBuffer, {}, {mBufferInfo});
		auto device = set.getDevice();
		device.updateDescriptorSets({write}, {});
	}

} // namespace BHive