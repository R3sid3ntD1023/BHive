#include "Buffers.h"
#include "Platform/GL/GLBuffers.h"

namespace BHive
{
	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* data, const uint32_t count)
	{
		return CreateRef<GLIndexBuffer>(data, count);
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t count)
	{
		return CreateRef<GLIndexBuffer>(count);
	}

	Ref<VertexBuffer> VertexBuffer::Create(const float* data, const uint64_t size)
	{
		return CreateRef<GLVertexBuffer>(data, size);
	}

	Ref<VertexBuffer> VertexBuffer::Create(const uint64_t size)
	{
		return CreateRef<GLVertexBuffer>(size);
	}
}