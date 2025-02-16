#include "StorageBuffer.h"
#include "Platform/GL/GLStorageBuffer.h"

namespace BHive
{

	Ref<StorageBuffer> StorageBuffer::Create(uint32_t binding, size_t size)
	{
		return CreateRef<GLStorageBuffer>(binding, size);
	}
} // namespace BHive