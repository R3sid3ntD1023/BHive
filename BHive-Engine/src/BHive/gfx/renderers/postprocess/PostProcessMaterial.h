#pragma once

#include "gfx/material/Material.h"
#include "gfx/RenderGraph.h"

namespace BHive
{
	class BHIVE_API PostProcessMaterial
	{
	public:
		virtual ~PostProcessMaterial() = default;

		virtual Ref<Texture> AddToGraph(RenderGraph &graph, const Ref<Texture>& input) = 0;

		virtual void CreateResizableObjects(const glm::uvec2 &size) = 0;

		virtual const char *GetName() const = 0;
	};

} // namespace BHive