#include "GLBuffers.h"
#include <glad/glad.h>

namespace BHive
{
	GLIndexBuffer::GLIndexBuffer(const uint32_t *data, const uint32_t count)
		: mCount(count)
	{
		glNamedBufferData(mBufferID, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
	}

	GLIndexBuffer::GLIndexBuffer(const uint32_t count)
		: mCount(count)
	{

		glNamedBufferData(mBufferID, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
	}

	void GLIndexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		GLBufferBase::SetData(data, size, offset);
	}

	GLVertexBuffer::GLVertexBuffer(const float *data, const uint64_t size)
	{
#ifdef USE_VERTEX_PULLING
		glNamedBufferStorage(mBufferID, size, data, 0);
#else
		glNamedBufferData(GetBufferID(), size, data, GL_STATIC_DRAW);
#endif
	}

	GLVertexBuffer::GLVertexBuffer(const uint64_t size)
	{
#ifdef USE_VERTEX_PULLING
		glNamedBufferStorage(mBufferID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
#else
		glNamedBufferData(GetBufferID(), size, nullptr, GL_DYNAMIC_DRAW);
#endif
	}

	void GLVertexBuffer::BindBufferBase(uint32_t binding) const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mBufferID);
	}

	void GLVertexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		GLBufferBase::SetData(data, size, offset);
	}

	void GLVertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}
} // namespace BHive