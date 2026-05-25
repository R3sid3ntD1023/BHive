#include "RenderCommand.h"
#include "Pipeline.h"

namespace BHive
{
	void RenderCommand::Init()
	{
		sRendererAPI->Init();
	}

	void RenderCommand::Shutdown()
	{
		sRendererAPI->Shutdown();
	}

	void RenderCommand::WaitIdle()
	{
		sRendererAPI->WaitIdle();
	}

	void RenderCommand::ClearColor(float r, float g, float b, float a)
	{
		sRendererAPI->ClearColor(r, g, b, a);
	}

	void RenderCommand::Clear(ClearMask mask)
	{
		sRendererAPI->Clear(mask);
	}

	void RenderCommand::SetLineWidth(float width)
	{
		sRendererAPI->SetLineWidth(width);
	}

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		sRendererAPI->SetViewport(x, y, w, h);
	}

	void RenderCommand::DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		sRendererAPI->DrawArrays(mode, vao, count);
	}

	void RenderCommand::DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		sRendererAPI->DrawElements(mode, vao, count);
	}

	void RenderCommand::DrawElementsBaseVertex(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{
		sRendererAPI->DrawElementsBaseVertex(mode, vao, start, start_index, count, instance_count);
	}

	void RenderCommand::DrawElementsRanged(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		sRendererAPI->DrawElementsRanged(mode, vao, start, end, count);
	}

	void RenderCommand::DrawElementsInstanced(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t instances, uint32_t count)
	{
		sRendererAPI->DrawElementsInstanced(mode, vao, instances, count);
	}

	void RenderCommand::MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const Ref<VertexArray> &vao, size_t drawCount, size_t stride)
	{
		sRendererAPI->MultiDrawElementsIndirect(mode, indirect, vao,  drawCount, stride);
	}

	void RenderCommand::ExecuteComputePass(const Ref<Pipeline> &pipeline, const glm::uvec3 &size, const FComputeFunc &builder)
	{
		sRendererAPI->ExecuteComputePass(pipeline, size, builder);
	}

	void RenderCommand::AddTransferPass(const std::string &name, const std::function<void(FRenderGraphPass &)> &builder)
	{
		RenderGraph graph{};
		auto &pass = graph.AddPass(name, EPassType::Transfer);

		auto *previous = sActivePass;
		sActivePass = &pass;

		builder(pass);

		FResourceUpdateList list{};
		RenderCommand::SubmitGraph(graph, list);

		sActivePass = previous;
	}

	
	void RenderCommand::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		sRendererAPI->ColorMask(r, g, b, a);
	}

	Ref<ISetManager> RenderCommand::CreateSetManager(const Pipeline *pipeline, uint32_t setIndex)
	{
		return sRendererAPI->CreateSetManager(pipeline, setIndex);
	}


	void RenderCommand::SubmitGraph(const RenderGraph &graph, FResourceUpdateList &updateResources)
	{
		sRendererAPI->SubmitGraph(graph, updateResources);
	}

	void RenderCommand::BeginFrame()
	{
		sGraph = RenderGraph{};
		sActivePass = nullptr;
		sFrameActive = true;
	}

	RenderGraph &RenderCommand::EndFrame()
	{
		sFrameActive = false;
		return sGraph;
	}

	RenderGraph &RenderCommand::GetActiveGraph()
	{
		if (!sFrameActive)
			BeginFrame();

		return sGraph;
	}

	FRenderGraphPass &RenderCommand::GetActivePass()
	{
		if (!sActivePass)
		{
			auto &graph = GetActiveGraph();
			sActivePass = &graph.AddPass(sPassConfig.DefaultPassName, sPassConfig.DefaultPassType);

			if (sPassConfig.DebugMarkers)
				DebugPass("AutoDefaultPass: " + sPassConfig.DefaultPassName);
		}

		return *sActivePass;
	}

	FRenderGraphPass &RenderCommand::BeginPass(const std::string &name, EPassType type)
	{
		auto& graph = GetActiveGraph();
		sActivePass = &graph.AddPass(name, type);

		if (sPassConfig.DebugMarkers)
			DebugPass("Begin Pass: " + name);

		return *sActivePass;
	}

	void RenderCommand::EndPass()
	{
		if (sActivePass && sPassConfig.DebugMarkers)
			DebugPass("EndPass: " + sActivePass->Name);

		sActivePass = nullptr;
	}

	void RenderCommand::SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd)
	{
		sRendererAPI->SubmitResourceUpdate(std::move(cmd));
	}

	void RenderCommand::SetPassConfig(const PassConfig &config)
	{
		sPassConfig = config;
	}

	void RenderCommand::DebugPass(const std::string &msg)
	{
		sRendererAPI->DebugPass(msg);
	}

	Scope<RendererAPI> RenderCommand::sRendererAPI = RendererAPI::Create();

} // namespace BHive