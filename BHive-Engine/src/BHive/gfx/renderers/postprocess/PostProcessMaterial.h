#pragma once

#include "gfx/RenderGraph.h"
#include "PostProcessAllocator.h"

namespace BHive
{
	class BHIVE_API PostProcessMaterial
	{
	public:
		virtual ~PostProcessMaterial() = default;

		virtual Ref<Texture> AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input) = 0;

		virtual void OnResize(const glm::uvec2 &size, PostProcessAllocator &allocator) {};

		virtual const char *GetName() const = 0;
	};

} // namespace BHive