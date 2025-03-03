#include "GLBufferBase.h"
#include <glad/glad.h>

namespace BHive
{
	GLBufferBase::GLBufferBase()
	{
		glCreateBuffers(1, &mBufferID);
	}

	GLBufferBase::~GLBufferBase()
	{
		glDeleteBuffers(1, &mBufferID);
	}

	void GLBufferBase::SetData(const void *data, size_t size, uint32_t offset)
	{
		glNamedBufferSubData(mBufferID, offset, size, data);
	}
} // namespace BHive