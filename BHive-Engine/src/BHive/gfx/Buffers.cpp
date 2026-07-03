#include "Buffers.h"
#include "Platform/Vulkan/VulkanBuffers.h"
#include "RenderCommand.h"

namespace BHive
{
	void IndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		RenderGraph graph{};
		auto &pass = graph.AddPass("BufferUpload", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Transfer);
		pass.Push(this, EBufferAccess::IndexRead);
		pass.Emplace<CmdUploadBuffer>()(this, data, size, offset);
		pass.EndPhase();

		Renderer::Get().ExecuteGraph(graph);
	}

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

	void VertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		RenderGraph graph{};
		auto &pass = graph.AddPass("BufferUpload", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Transfer);
		pass.Push(this, EBufferAccess::VertexRead);
		pass.Emplace<CmdUploadBuffer>()(this, data, size, offset);
		pass.EndPhase();

		Renderer::Get().ExecuteGraph(graph);
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

	void GPUBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		auto &graph = Renderer::Get().GetActiveGraph();
		auto &pass = graph.AddPass("BufferUpload", EPassType::OffScreen);

		pass.BeginPhase(EPhaseType::Transfer);
		pass.Push(this, EBufferAccess::TransferWrite);
		pass.Emplace<CmdUploadBuffer>()(this, data, size, offset);
		pass.EndPhase();
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