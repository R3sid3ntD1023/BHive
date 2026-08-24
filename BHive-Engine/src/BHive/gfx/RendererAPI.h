#pragma once

#include "core/Core.h"
#include "Enumerations.h"
#include "RenderGraph.h"

namespace BHive
{
#define MULTI_DRAW_INDIRECT_STRIDE sizeof(MultiDrawIndirectCommand)

	class WindowContext;

	struct MultiDrawIndirectCommand
	{
		uint32_t indexCount = 0;
		uint32_t instanceCount = 0;
		uint32_t firstIndex = 0;
		int32_t vertexOffset = 0;
		uint32_t firstInstance = 0;
	};

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

		static Scope<RendererAPI> Create();
	};
} // namespace BHive