#include "UniformBuffer.h"
#include "Platform/GL/GLUniformBuffer.h"

namespace BHive
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t binding, uint64_t size)
	{
		return CreateRef<GLUniformBuffer>(binding, size);
	}

    Ref<UniformBuffer> UniformBuffer::Create(uint64_t size, const void *data)
    {
        return CreateRef<GLUniformBuffer>(size, data);
    }
}