#include "GLStorageBuffer.h"
#include <glad/glad.h>

namespace BHive
{
	GLStorageBuffer::GLStorageBuffer(uint32_t binding, size_t size)
	{
		glCreateBuffers(1, &mBufferID);
		glNamedBufferStorage(mBufferID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mBufferID);
	}

	GLStorageBuffer::GLStorageBuffer(size_t size)
	{
		glCreateBuffers(1, &mBufferID);
		glNamedBufferStorage(mBufferID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	GLStorageBuffer::GLStorageBuffer(const void *data, size_t size)
	{
		glCreateBuffers(1, &mBufferID);
		glNamedBufferStorage(mBufferID, size, data, 0);
	}

	GLStorageBuffer::~GLStorageBuffer()
	{
		glDeleteBuffers(1, &mBufferID);
	}

	void GLStorageBuffer::BindBufferBase(size_t binding)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mBufferID);
	}

	void GLStorageBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		glNamedBufferSubData(mBufferID, offset, size, data);
	}
} // namespace BHive