#include "GLUniformBuffer.h"
#include <glad/glad.h>
#include "threading/Threading.h"

namespace BHive
{
	GLUniformBuffer::GLUniformBuffer(uint32_t binding, uint64_t size)
	{

		glCreateBuffers(1, &mUniformBufferID);
		glNamedBufferData(mUniformBufferID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mUniformBufferID);
	}

	GLUniformBuffer::GLUniformBuffer(uint64_t size, const void *data)
	{

		glCreateBuffers(1, &mUniformBufferID);
		glNamedBufferData(mUniformBufferID, size, data, GL_DYNAMIC_DRAW);
	}

	GLUniformBuffer::~GLUniformBuffer()
	{
		auto id = mUniformBufferID;

		glDeleteBuffers(1, &id);
	}

	void GLUniformBuffer::Bind(uint32_t binding) const
	{

		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mUniformBufferID);
	}

	void GLUniformBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{

		glNamedBufferSubData(mUniformBufferID, offset, size, data);
	}
}