#include "Buffers.h"
#include "VulkanUtils.h"

namespace BHive
{
	IndexBuffer::IndexBuffer(const uint32_t *data, const uint32_t count)
		: mCount(count)
	{
		
	}

	IndexBuffer::IndexBuffer(const uint32_t count)
		: mCount(count)
	{

		
	}

	void IndexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		BufferBase::SetData(data, size, offset);
	}

	VertexBuffer::VertexBuffer(const float *data, const uint64_t size): VertexBuffer(size * sizeof(float))
	{
		SetData(data, size * sizeof(float), 0);
	}

	VertexBuffer::VertexBuffer(const uint64_t size)
	{
		VulkanUtils::CreateBuffer(
			size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mBuffer, mMemory);
	
	}

	void VertexBuffer::BindBufferBase(uint32_t binding) const
	{
		
	}

	void VertexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		vk::raii::DeviceMemory stagingBufferMemory = nullptr;
		vk::raii::Buffer stagingBuffer = nullptr;

		VulkanUtils::CreateBuffer(
			size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

		void *stagingdata = stagingBufferMemory.mapMemory(offset, size);
		memcpy(stagingdata, data, size);
		stagingBufferMemory.unmapMemory();

		VulkanUtils::CopyBuffer(stagingBuffer, mBuffer, size);
	}

	void VertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

} // namespace BHive