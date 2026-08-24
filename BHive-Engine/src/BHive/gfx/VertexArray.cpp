#include "Platform/Vulkan/VulkanVertexArray.h"
#include "RenderCommand.h"
#include "VertexArray.h"
#include "rendergraph/Pass.h"

namespace BHive
{
	void VertexArray::DeclareAccess(FPass &pass, EBufferUsage vbAccess, EBufferUsage ibAccess)
	{
		for (auto &vb : GetVertexBuffers())
			pass.UseBuffer(vb, vbAccess);

		if (auto &ib = GetIndexBuffer())
			pass.UseBuffer(ib, ibAccess);
	}

	Ref<VertexArray> VertexArray::Create()
	{
		switch (RenderCommand::GetAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateRef<VulkanVertexArray>();
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<VertexArray> VertexArray::Create(const std::vector<Ref<VertexBuffer>> &vertex_buffers, const Ref<IndexBuffer> &index_buffer)
	{
		switch (RenderCommand::GetAPI())
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