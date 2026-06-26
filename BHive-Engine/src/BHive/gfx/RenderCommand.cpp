#include "RenderCommand.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void RenderCommand::Init(RendererAPI::EAPI apiType)
	{
		sAPI = apiType;
	}

	void RenderCommand::Submit(CommandFn &&fn)
	{
		sQueue.emplace_back(std::move(fn));
	}

	void RenderCommand::Flush(RendererAPI *api)
	{
		for (auto &fn : sQueue)
			fn(api);

		sQueue.clear();
	}

	void RenderCommand::SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd)
	{
		Renderer::Get().SubmitResourceUpdate(std::move(cmd));
	}

	void RenderCommand::QueueDeletion(FQeueuDeflectionFunc &&fn)
	{
		Renderer::Get().GetGraphicsAPI()->QueueDeletion(std::move(fn));
	}

	void RenderCommand::BeginFrame()
	{
		Renderer::Get().BeginFrame();
	}

	FPass &RenderCommand::BeginPass(const std::string &name, EPassType type)
	{
		return Renderer::Get().BeginPass(name, type);
	}

	void RenderCommand::EndPass()
	{
		Renderer::Get().EndPass();
	}

	void RenderCommand::EndFrame()
	{
		Renderer::Get().EndFrame();
	}

	RendererAPI *RenderCommand::GetGraphicsAPI()
	{
		return Renderer::Get().GetGraphicsAPI();
	}

	FPass &RenderCommand::GetActivePass()
	{
		return Renderer::Get().GetActivePass();
	}

} // namespace BHive