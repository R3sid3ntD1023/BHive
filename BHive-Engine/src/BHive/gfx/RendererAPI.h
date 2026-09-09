#pragma once

#include "Enumerations.h"
#include "core/Core.h"
#include "gfx/rendergraph/Graph.h"

namespace BHive
{
	using FQeueuDeletionFunc = std::function<void(uint32_t)>;

	class BHIVE_API RendererAPI
	{
	public:
		enum EAPI
		{
			Opengl,
			Vulkan
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		virtual void Shutdown() = 0;

		virtual void SubmitGraph(const Graph &graph) = 0;

		virtual void QueueDeletion(FQeueuDeletionFunc &&fn) = 0;

		static Scope<RendererAPI> Create();
	};
} // namespace BHive