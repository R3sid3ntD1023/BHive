#include "Platform/Vulkan/VulkanVertexArray.h"
#include "RenderCommand.h"
#include "VertexArray.h"

namespace BHive
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateRef<VulkanVertexArray>();
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<VertexArray> VertexArray::Create(const std::vector<Ref<VertexBuffer>>& vertex_buffers, const Ref<IndexBuffer> &index_buffer)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateRef<VulkanVertexArray>(vertex_buffers, index_buffer);
		}

		ASSERT(false);
		return nullptr;
	}
} // namespace BHive