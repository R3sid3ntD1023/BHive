#include "gfx/RenderCommand.h"
#include "VulkanUniformBuffer.h"
#include "VulkanUtils.h"

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
		if (!data)
			return;

		mBuffer.SetData(data, size, offset);
	}

} // namespace BHive