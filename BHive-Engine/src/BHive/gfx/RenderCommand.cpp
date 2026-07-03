#include "RenderCommand.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void RenderCommand::Init(RendererAPI::EAPI apiType)
	{
		sAPI = apiType;
	}

	void RenderCommand::QueueDeletion(FQeueuDeflectionFunc &&fn)
	{
		Renderer::Get().GetGraphicsAPI()->QueueDeletion(std::move(fn));
	}

	RendererAPI *RenderCommand::GetGraphicsAPI()
	{
		return Renderer::Get().GetGraphicsAPI();
	}

} // namespace BHive