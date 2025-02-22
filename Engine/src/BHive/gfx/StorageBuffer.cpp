#include "StorageBuffer.h"
#include "Platform/GL/GLStorageBuffer.h"

namespace BHive
{

	Ref<StorageBuffer> StorageBuffer::Create(uint32_t binding, size_t size)
	{
		return CreateRef<GLStorageBuffer>(binding, size);
	}

	Ref<StorageBuffer> StorageBuffer::Create(size_t size)
	{
		return CreateRef<GLStorageBuffer>(size);
	}
	Ref<StorageBuffer> StorageBuffer::Create(const void *data, size_t size)
	{
		return CreateRef<GLStorageBuffer>(data, size);
	}
} // namespace BHive