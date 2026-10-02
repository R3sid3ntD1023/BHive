#include "BufferFactory.h"
#include "Platform/Vulkan/VulkanVertexArray.h"
#include "gfx/Buffers.h"
#include "gfx/RenderCommand.h"
#include "gfx/VertexArray.h"

namespace BHive
{
	IndexBufferPtr BufferFactory::CreateIndexBuffer(uint32_t count, EBufferLifetime lifeTime, const uint32_t *data)
	{
		return CreateResource<IndexBuffer>(count * sizeof(uint32_t), lifeTime, data, count);
	}

	VertexBufferPtr BufferFactory::CreateVertexBuffer(size_t size, EBufferLifetime lifeTime, const void *data)
	{
		return CreateResource<VertexBuffer>(size, lifeTime, data, size);
	}

	BufferPtr BufferFactory::Create(size_t size, EBufferType usage, EBufferLifetime lifeTime, const void *data)
	{
		return CreateResource<GeneralBuffer>(size, usage, lifeTime, data, size);
	}

	VertexArrayPtr VertexArrayFactory::Create()
	{
		switch (RenderCommand::GetAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateResource<VulkanVertexArray>();
		}

		ASSERT(false);
		return {};
	}

	VertexArrayPtr VertexArrayFactory::Create(const std::vector<VertexBufferPtr> &vbos, IndexBufferPtr ibo)
	{
		switch (RenderCommand::GetAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateResource<VulkanVertexArray>(vbos, ibo);
		}

		ASSERT(false);
		return {};
	}
} // namespace BHive