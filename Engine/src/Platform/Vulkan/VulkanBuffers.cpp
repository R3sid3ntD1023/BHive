#include "gfx/VulkanUtils.h"
#include "VulkanBuffers.h"

namespace BHive
{

	VulkanIndexBuffer::VulkanIndexBuffer(const uint32_t count, const uint32_t *data)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mCount(count)
	{
		VulkanUtils::CreateBuffer(count * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mBuffer);
		SetData(data, count * sizeof(uint32_t));
	}

	void VulkanIndexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		if (!data)
			return;

		AllocatedVulkanBuffer stagingBuffer;

		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer);

		stagingBuffer.SetData(data, size, 0);

		VulkanUtils::CopyBuffer(stagingBuffer, mBuffer, size);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const uint64_t size, const float *data)
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mBuffer);
		SetData(data, size * sizeof(float), 0);
	}

	void VulkanVertexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		if (!data)
			return;

		AllocatedVulkanBuffer stagingBuffer;

		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer);

		stagingBuffer.SetData(data, size, offset);

		VulkanUtils::CopyBuffer(stagingBuffer, mBuffer, size);
	}

	void VulkanVertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}
} // namespace BHive