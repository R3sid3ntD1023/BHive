#include "Buffers.h"
#include "Platform/Vulkan/VulkanBuffers.h"
#include "RenderCommand.h"

namespace BHive
{

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t count, EBufferUsageType usage, const uint32_t *data)
	{
		switch (RenderCommand::GetAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			if (usage == EBufferUsageType::Static)
			{
				ASSERT(data, "Static buffers must be created with initial data!");

				return CreateRef<StaticVulkanIndexBuffer>(data, count);
			}
			else
				return CreateRef<DynamicVulkanIndexBuffer>(data, count);
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<VertexBuffer> BHive::VertexBuffer::Create(const uint64_t size, EBufferUsageType usage, const void *data)
	{
		switch (RenderCommand::GetAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			if (usage == EBufferUsageType::Static)
			{
				ASSERT(data , "Static buffers must be created with initial data!");
				return CreateRef<StaticVulkanVertexBuffer>(data, size);
			}
			else
				return CreateRef<DynamicVulkanVertexBuffer>(data, size);
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<GPUBuffer> GPUBuffer::Create(size_t size, EBufferType usage, const void *data)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanGPUBuffer>(size, usage, data);
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}

} // namespace BHive