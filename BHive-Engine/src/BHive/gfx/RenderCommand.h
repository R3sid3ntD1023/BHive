#pragma once

#include "core/Core.h"
#include "gfx/RenderGraph.h"
#include "gfx/RendererAPI.h"

namespace BHive
{	
	class RendererAPI;

	class BHIVE_API RenderCommand
	{
	public:
		
		using CommandFn = std::function<void(RendererAPI*)>;

		static void Init(RendererAPI::EAPI apiType);

		static void Submit(CommandFn &&fn);

		static void Flush(RendererAPI *api);

		template<typename Callable>
		static void SubmitCommand(const std::string& name, Callable&& fn)
		{
			auto &pass = GetActivePass();

			pass.Push(name, std::move(fn));
		}

		static void SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd);

		static void QueueDeletion(FQeueuDeflectionFunc &&fn);

		static void BeginFrame();

		static FPass &BeginPass(const std::string &name, EPassType type);

		static void EndPass();

		static void EndFrame();

		static RendererAPI::EAPI GetAPI() { return sAPI; }

		static RendererAPI *GetGraphicsAPI();

		template<typename T>
		static T *GetGraphicsAPI()
		{
			return Cast<T>(GetGraphicsAPI());
		}

	private:
		static FPass &GetActivePass();

	private:
		static inline std::vector<CommandFn> sQueue;

		static inline RendererAPI::EAPI sAPI;
	};
} // namespace BHive
