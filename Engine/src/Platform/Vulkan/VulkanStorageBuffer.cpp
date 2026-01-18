#include "VulkanStorageBuffer.h"
#include "VulkanUtils.h"

namespace BHive
{

	VulkanStorageBuffer::VulkanStorageBuffer(uint32_t binding, size_t size, const void *data)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mBinding(binding)
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible, mBuffer);

		SetData(data, size, 0);

		mBufferInfo = vk::DescriptorBufferInfo(mBuffer.Buffer, 0, size);
	}

	VulkanStorageBuffer::VulkanStorageBuffer(size_t size)
		: mDevice(VulkanCore::GetLogicalDevice())
	{
	}

	void VulkanStorageBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		mBuffer.SetData(data, size, offset);
	}

} // namespace BHive