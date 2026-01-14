#include "Buffers.h"
#include "VulkanUtils.h"

namespace BHive
{
	IndexBuffer::IndexBuffer(const uint32_t *data, const uint32_t count)
		: IndexBuffer(count)
	{
		SetData(data, count * sizeof(uint32_t));
	}

	IndexBuffer::IndexBuffer(const uint32_t count)
		: mCount(count)
	{
		VulkanUtils::CreateBuffer(count * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mBuffer);
	}

	void IndexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		AllocatedVulkanBuffer stagingBuffer;

		VulkanUtils::CreateBuffer(
			size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer);

		stagingBuffer.Map(size);
		stagingBuffer.SetData(data, size, 0);

		VulkanUtils::CopyBuffer(stagingBuffer, mBuffer, size);
	}

	VertexBuffer::VertexBuffer(const float *data, const uint64_t size)
		: VertexBuffer(size * sizeof(float))
	{
		SetData(data, size * sizeof(float), 0);
	}

	VertexBuffer::VertexBuffer(const uint64_t size)
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mBuffer);
	}

	void VertexBuffer::BindBufferBase(uint32_t binding) const
	{
	}

	void VertexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		AllocatedVulkanBuffer stagingBuffer;

		VulkanUtils::CreateBuffer(
			size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer);

		stagingBuffer.Map(size);
		stagingBuffer.SetData(data, size, offset);

		VulkanUtils::CopyBuffer(stagingBuffer, mBuffer, size);
	}

	void VertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

} // namespace BHive