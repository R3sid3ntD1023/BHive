#include "BufferFactory.h"
#include "Platform/Vulkan/VulkanBuffers.h"
#include "Platform/Vulkan/VulkanVertexArray.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	IndexBufferPtr BufferFactory::CreateIndexBuffer(const uint32_t count, EBufferLifetime lifeTime, const uint32_t *data)
	{
		switch (RenderCommand::GetAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateResource<VulkanIndexBuffer>(count, lifeTime, data);
		}

		ASSERT(false);
		return {};
	}

	VertexBufferPtr BufferFactory::CreateVertexBuffer(const uint64_t size, EBufferLifetime lifeTime, const void *data)
	{
		switch (RenderCommand::GetAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateResource<VulkanVertexBuffer>(size, lifeTime, data);
		}

		ASSERT(false);
		return {};
	}

	BufferPtr BufferFactory::Create(size_t size, EBufferType usage, EBufferLifetime lifeTime, const void *data)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateResource<VulkanGeneralBuffer>(size, usage, lifeTime, data);
		default:
			break;
		}

		ASSERT(false)
		return {};
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