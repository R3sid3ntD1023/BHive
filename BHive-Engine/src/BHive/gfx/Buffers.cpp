#include "Buffers.h"
#include "Platform/Vulkan/VulkanBuffers.h"
#include "RenderCommand.h"

namespace BHive
{
	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t count, EBufferLifetime lifeTime, const uint32_t *data)
	{
		switch (RenderCommand::GetAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateRef<VulkanIndexBuffer>(count, lifeTime, data);
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<VertexBuffer> BHive::VertexBuffer::Create(const uint64_t size, EBufferLifetime lifeTime, const void *data)
	{
		switch (RenderCommand::GetAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateRef<VulkanVertexBuffer>(size, lifeTime, data);
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<GeneralBuffer> GeneralBuffer::Create(size_t size, EBufferType usage, EBufferLifetime lifeTime, const void *data)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanGeneralBuffer>(size, usage, lifeTime, data);
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}

} // namespace BHive