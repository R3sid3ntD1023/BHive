#pragma once

#include "core/Core.h"
#include "Enumerations.h"
#include "RenderGraph.h"

namespace BHive
{
	class WindowContext;


	struct MultiDrawIndirectCommand
	{
		uint32_t Count;
		uint32_t InstanceCount;
		uint32_t FirstIndex;
		int32_t BaseVertex;
		uint32_t BaseInstance;
	};

	using FComputeFunc = std::function<void(FComputeBindings &)>;
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

		virtual void SubmitGraph(const RenderGraph &graph) = 0;

		virtual void QueueDeletion(FQeueuDeletionFunc &&fn) = 0;

		virtual void SetCurrentContext(WindowContext *ctx) = 0;

		virtual WindowContext *GetCurrentContext() const = 0;

		static Scope<RendererAPI> Create();
	};
} // namespace BHive