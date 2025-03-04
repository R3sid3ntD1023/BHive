#include "UniformBuffer.h"
#include <glad/glad.h>

namespace BHive
{
	UniformBuffer::UniformBuffer(uint32_t binding, uint64_t size)
	{
		glNamedBufferData(mBufferID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mBufferID);
	}

	UniformBuffer::UniformBuffer(uint64_t size, const void *data)
	{
		glNamedBufferData(mBufferID, size, data, GL_DYNAMIC_DRAW);
	}

	void UniformBuffer::BindBufferBase(uint32_t binding) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mBufferID);
	}

} // namespace BHive