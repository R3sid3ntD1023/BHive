#include "Buffers.h"
#include "Platform/Vulkan/VulkanBuffers.h"
#include "RenderCommand.h"
#include "VulkanUtils.h"

namespace BHive
{

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t count, const uint32_t *data)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateRef<VulkanIndexBuffer>(count, data);
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<VertexBuffer> BHive::VertexBuffer::Create(const uint64_t size, const float *data)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateRef<VulkanVertexBuffer>(size, data);
		}

		ASSERT(false);
		return nullptr;
	}

} // namespace BHive