#include "GLUniformBuffer.h"
#include <glad/glad.h>
#include "threading/Threading.h"

namespace BHive
{
	GLUniformBuffer::GLUniformBuffer(uint32_t binding, uint64_t size)
	{
		BEGIN_THREAD_DISPATCH(=)
		glCreateBuffers(1, &mUniformBufferID);
		glNamedBufferData(mUniformBufferID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mUniformBufferID);
		END_THREAD_DISPATCH()
	}

	GLUniformBuffer::GLUniformBuffer(uint64_t size, const void *data)
	{
		BEGIN_THREAD_DISPATCH(=)
		glCreateBuffers(1, &mUniformBufferID);
		glNamedBufferData(mUniformBufferID, size, data, GL_DYNAMIC_DRAW);
		END_THREAD_DISPATCH()
	}

	GLUniformBuffer::~GLUniformBuffer()
	{
		auto id = mUniformBufferID;
		BEGIN_THREAD_DISPATCH(=)
		glDeleteBuffers(1, &id);
		END_THREAD_DISPATCH()
	}

	void GLUniformBuffer::Bind(uint32_t binding) const
	{
		BEGIN_THREAD_DISPATCH(=)
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mUniformBufferID);
		END_THREAD_DISPATCH()
	}

	void GLUniformBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		BEGIN_THREAD_DISPATCH(=)
		glNamedBufferSubData(mUniformBufferID, offset, size, data);
		END_THREAD_DISPATCH()
	}
}