#include "StorageBuffer.h"
#include <glad/glad.h>

namespace BHive
{
	StorageBuffer::StorageBuffer(uint32_t binding, size_t size)
	{
		glNamedBufferStorage(mBufferID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mBufferID);
	}

	StorageBuffer::StorageBuffer(size_t size)
	{
		glNamedBufferStorage(mBufferID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	StorageBuffer::StorageBuffer(const void *data, size_t size)
	{
		glNamedBufferStorage(mBufferID, size, data, 0);
	}

	void StorageBuffer::BindBufferBase(uint32_t binding) const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mBufferID);
	}

} // namespace BHive