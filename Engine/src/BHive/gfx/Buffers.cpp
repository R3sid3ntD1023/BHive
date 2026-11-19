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

	VertexBuffer::VertexBuffer(const float *data, const uint64_t size)
	{

		glNamedBufferData(GetBufferID(), size, data, GL_STATIC_DRAW);
	}

	VertexBuffer::VertexBuffer(const uint64_t size)
	{
		glNamedBufferData(GetBufferID(), size, nullptr, GL_DYNAMIC_DRAW);
	}

	void VertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

} // namespace BHive