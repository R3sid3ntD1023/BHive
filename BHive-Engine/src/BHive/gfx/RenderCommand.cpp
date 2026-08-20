#include "RenderCommand.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void RenderCommand::Init(RendererAPI::EAPI apiType)
	{
		sAPI = apiType;
	}

	void RenderCommand::Shutdown()
	{
		sIsShuttingDown = true;
	}

	void RenderCommand::QueueDeletion(FQeueuDeletionFunc &&fn)
	{
		auto api = Renderer::Get().GetGraphicsAPI();
		if (!api || sIsShuttingDown)
		{
			fn(UINT32_MAX);
			// LOG_WARN("QueueDeletion called after shutdown; ignoring.");
			return;
		}

		api->QueueDeletion(std::move(fn));
	}

	RendererAPI *RenderCommand::GetGraphicsAPI()
	{
		return Renderer::Get().GetGraphicsAPI();
	}

} // namespace BHive