#pragma once

#include "IResourceFactory.h"
#include "gfx/Buffers.h"
#include "gfx/VertexArray.h"

namespace BHive
{
	struct BHIVE_API BufferFactory : public IResourceFactory<BufferBase>
	{
		static BufferPtr Create(size_t size, EBufferType type, EBufferLifetime lifetime = EBufferLifetime::Dynamic, const void *data = nullptr);

		static IndexBufferPtr CreateIndexBuffer(const uint32_t count, EBufferLifetime lifetime = EBufferLifetime::Dynamic, const uint32_t *data = nullptr);

		static VertexBufferPtr CreateVertexBuffer(const uint64_t size, EBufferLifetime lifetime = EBufferLifetime::Dynamic, const void *data = nullptr);
	};

	struct BHIVE_API VertexArrayFactory : public IResourceFactory<VertexArray>
	{
		static VertexArrayPtr Create();

		static VertexArrayPtr Create(const std::vector<VertexBufferPtr> &vbos, IndexBufferPtr ibo = {});
	};
} // namespace BHive