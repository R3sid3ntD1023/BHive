#include "BufferBase.h"
#include "BufferBase.h"
#include <glad/glad.h>

namespace BHive
{
	BufferBase::BufferBase()
	{
		glCreateBuffers(1, &mBufferID);
	}

	BufferBase::~BufferBase()
	{
		glDeleteBuffers(1, &mBufferID);
	}

	void BufferBase::SetData(const void *data, size_t size, uint32_t offset)
	{
		glNamedBufferSubData(mBufferID, offset, size, data);
	}

	void BHive::BufferBase::BindBufferBase(uint32_t binding) const
	{
	}
} // namespace BHive