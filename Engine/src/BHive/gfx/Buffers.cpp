#include "Buffers.h"
#include <glad/glad.h>

namespace BHive
{
	IndexBuffer::IndexBuffer(const uint32_t *data, const uint32_t count)
		: mCount(count)
	{
		glNamedBufferData(mBufferID, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
	}

	IndexBuffer::IndexBuffer(const uint32_t count)
		: mCount(count)
	{

		glNamedBufferData(mBufferID, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
	}

	void IndexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		BufferBase::SetData(data, size, offset);
	}

	VertexBuffer::VertexBuffer(const float *data, const uint64_t size)
	{
#ifdef USE_VERTEX_PULLING
		glNamedBufferStorage(mBufferID, size, data, 0);
#else
		glNamedBufferData(GetBufferID(), size, data, GL_STATIC_DRAW);
#endif
	}

	VertexBuffer::VertexBuffer(const uint64_t size)
	{
#ifdef USE_VERTEX_PULLING
		glNamedBufferStorage(mBufferID, size, nullptr, _DYNAMIC_STORAGE_BIT);
#else
		glNamedBufferData(GetBufferID(), size, nullptr, GL_DYNAMIC_DRAW);
#endif
	}

	void VertexBuffer::BindBufferBase(uint32_t binding) const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mBufferID);
	}

	void VertexBuffer::SetData(const void *data, uint64_t size, uint32_t offset)
	{
		BufferBase::SetData(data, size, offset);
	}

	void VertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}
} // namespace BHive