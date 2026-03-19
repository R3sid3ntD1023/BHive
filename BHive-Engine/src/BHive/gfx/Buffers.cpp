#include "Buffers.h"
#include "Platform/Vulkan/VulkanBuffers.h"
#include "RenderCommand.h"

namespace BHive
{

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t count, EBufferUsageType usage)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			if (usage == EBufferUsageType::Static)
				return CreateRef<StaticVulkanIndexBuffer>(count);
			else
				return CreateRef<DynamicVulkanIndexBuffer>(count);
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<VertexBuffer> BHive::VertexBuffer::Create(const uint64_t size,  EBufferUsageType usage)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			if (usage == EBufferUsageType::Static)
				return CreateRef<StaticVulkanVertexBuffer>(size);
			else
				return CreateRef<DynamicVulkanVertexBuffer>(size);
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<GPUBuffer> GPUBuffer::Create(size_t size, EBufferType usage, const void *data)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanGPUBuffer>(size, usage, data);
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}

	Ref<GPUBuffer> GPUBuffer::Create(size_t size, EBufferType usage)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanGPUBuffer>(size, usage);
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}

} // namespace BHive