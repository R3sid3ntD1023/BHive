#include "VulkanBuffers.h"
#include "VulkanConversions.h"
#include "VulkanUtils.h"

namespace BHive
{

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t count, EBufferLifetime lifeTime, const uint32_t *data)
		: mCount(count)
	{
		mBuffer.Init(count * sizeof(uint32_t), data, vk::BufferUsageFlagBits::eIndexBuffer, lifeTime);
	}

	void VulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		mBuffer.Upload(data, size, offset);
	}

	void VulkanIndexBuffer::Clear()
	{
		mBuffer.ClearData();
	}

	VulkanVertexBuffer::VulkanVertexBuffer(size_t size, EBufferLifetime lifeTime, const void *data)
	{
		mBuffer.Init(size, data, vk::BufferUsageFlagBits::eVertexBuffer, lifeTime);
	}

	void VulkanVertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		mBuffer.Upload(data, size, offset);
	}

	void VulkanVertexBuffer::Clear()
	{
		mBuffer.ClearData();
	}

	VulkanGeneralBuffer::VulkanGeneralBuffer(size_t size, EBufferType type, EBufferLifetime lifeTime, const void *data)
	{
		mBuffer.Init(size, data, ToVkBufferType(type), lifeTime);
	}

	void VulkanGeneralBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		mBuffer.Upload(data, size, offset);
	}

	void VulkanGeneralBuffer::Clear()
	{
		mBuffer.ClearData();
	}
} // namespace BHive