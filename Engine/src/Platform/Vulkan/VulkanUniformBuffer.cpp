#include "gfx/VulkanUtils.h"
#include "VulkanUniformBuffer.h"

namespace BHive
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t binding, uint64_t size, const void *data)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mBinding(binding)
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible, mBuffer);

		SetData(data, size, 0);

		mBufferInfo = vk::DescriptorBufferInfo(mBuffer.Buffer, 0, size);
	}

	void VulkanUniformBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		mBuffer.SetData(data, size, offset);
	}

	void VulkanUniformBuffer::WriteDescriptor(const vk::raii::DescriptorSet &set)
	{
		vk::WriteDescriptorSet writer(set, mBinding, 0, vk::DescriptorType::eUniformBuffer, {}, {mBufferInfo});
		mDevice.updateDescriptorSets(writer, nullptr);
	}

} // namespace BHive