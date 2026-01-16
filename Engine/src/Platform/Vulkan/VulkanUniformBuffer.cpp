#include "gfx/VulkanUtils.h"
#include "VulkanUniformBuffer.h"

namespace BHive
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t binding, uint64_t size, const void *data)
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible, mBuffer);

		if (data)
		{
			SetData(data, size, 0);
		}

		mBufferInfo = vk::DescriptorBufferInfo(mBuffer.Buffer, 0, vk::DeviceSize(size));
	}

	void VulkanUniformBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		mBuffer.SetData(data, size, offset);
	}

} // namespace BHive