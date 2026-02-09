#include "Buffers.h"
#include "Platform/Vulkan/VulkanBuffers.h"
#include "RenderCommand.h"

namespace BHive
{

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t count, EBufferUsage usage)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			if (usage == EBufferUsage::Static)
				return CreateRef<StaticVulkanIndexBuffer>(count);
			else
				return CreateRef<DynamicVulkanIndexBuffer>(count);
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<VertexBuffer> BHive::VertexBuffer::Create(const uint64_t size,  EBufferUsage usage)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			if (usage == EBufferUsage::Static)
				return CreateRef<StaticVulkanVertexBuffer>(size);
			else
				return CreateRef<DynamicVulkanVertexBuffer>(size);
		}

		ASSERT(false);
		return nullptr;
	}

} // namespace BHive