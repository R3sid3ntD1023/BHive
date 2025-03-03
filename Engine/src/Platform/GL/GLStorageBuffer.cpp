#include "GLStorageBuffer.h"
#include <glad/glad.h>

namespace BHive
{
	GLStorageBuffer::GLStorageBuffer(uint32_t binding, size_t size)
	{
		glNamedBufferStorage(mBufferID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mBufferID);
	}

	GLStorageBuffer::GLStorageBuffer(size_t size)
	{
		glNamedBufferStorage(mBufferID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	GLStorageBuffer::GLStorageBuffer(const void *data, size_t size)
	{
		glNamedBufferStorage(mBufferID, size, data, 0);
	}

	void GLStorageBuffer::BindBufferBase(size_t binding)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mBufferID);
	}

	void GLStorageBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		GLBufferBase::SetData(data, size, offset);
	}
} // namespace BHive