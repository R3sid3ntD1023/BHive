#include "GLStorageBuffer.h"
#include <glad/glad.h>

namespace BHive
{
	GLStorageBuffer::GLStorageBuffer(uint32_t binding, size_t size)
	{
		glCreateBuffers(1, &mBufferID);
		glNamedBufferData(mBufferID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mBufferID);
	}

	GLStorageBuffer::~GLStorageBuffer()
	{
		glDeleteBuffers(1, &mBufferID);
	}

	void GLStorageBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		glNamedBufferSubData(mBufferID, offset, size, data);
	}
} // namespace BHive