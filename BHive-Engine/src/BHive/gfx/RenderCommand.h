#pragma once

#include "core/Core.h"
#include "gfx/RenderGraph.h"
#include "renderers/Renderer.h"
#include "gfx/RendererAPI.h"

namespace BHive
{
	class RendererAPI;

	class BHIVE_API RenderCommand
	{
	public:
		static void Init(RendererAPI::EAPI apiType);

		static void Shutdown();

		static void QueueDeletion(FQeueuDeletionFunc &&fn);

		static RendererAPI::EAPI GetAPI() { return sAPI; }

		static RendererAPI *GetGraphicsAPI();

		static bool IsShuttingDown() { return sIsShuttingDown; }

		template <typename T>
		static T *GetGraphicsAPI()
		{
			return Cast<T>(GetGraphicsAPI());
		}

	private:
		static inline RendererAPI::EAPI sAPI;

		static inline bool sIsShuttingDown = false;
	};
} // namespace BHive
