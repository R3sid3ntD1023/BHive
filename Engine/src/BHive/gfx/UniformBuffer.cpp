#include "DescriptorBuilder.h"
#include "UniformBuffer.h"
#include "VulkanUtils.h"

namespace BHive
{
	UniformBuffer::UniformBuffer(uint32_t binding, uint64_t size, const void *data)
		: mBinding(binding)
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mBuffer);
		if (data)
		{
			SetData(data, size, 0);
		}

		mBufferInfo = vk::DescriptorBufferInfo(mBuffer.Buffer, 0, vk::DeviceSize(size));
	}

	void UniformBuffer::SetData(const void* data, size_t size, uint32_t offset)
	{
		mBuffer.SetData(data, size, offset);
	}

	void UniformBuffer::WriteDescriptor(const vk::raii::DescriptorSet &set) const
	{
		vk::WriteDescriptorSet write(set, mBinding, 0, vk::DescriptorType::eUniformBuffer, {}, {mBufferInfo});
		auto device = set.getDevice();
		device.updateDescriptorSets({write}, {});
	}

} // namespace BHive