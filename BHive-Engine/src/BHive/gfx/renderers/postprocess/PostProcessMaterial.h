#pragma once

#include "gfx/RenderGraph.h"

namespace BHive
{
	struct BHIVE_API FPostProcessTextureSet
	{
		TexturePtr SceneColor;
		TexturePtr SceneDepth;
		TexturePtr PrevOutput;
	};

	class BHIVE_API PostProcessMaterial
	{
	public:
		virtual ~PostProcessMaterial() = default;

		virtual void Init(const glm::uvec2 &size) = 0;

		virtual void Resize(const glm::uvec2 &size) = 0;

		virtual TexturePtr AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set) = 0;
	};

} // namespace BHive