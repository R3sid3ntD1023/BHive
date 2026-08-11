#pragma once

#include "gfx/RenderGraph.h"

namespace BHive
{
	struct BHIVE_API FPostProcessTextureSet
	{
		Ref<Texture> SceneColor;
		Ref<Texture> SceneDepth;
		Ref<Texture> PrevOutput;
	};

	class BHIVE_API PostProcessMaterial
	{
	public:
		virtual ~PostProcessMaterial() = default;

		virtual void Init(const glm::uvec2 &size) = 0;

		virtual Ref<Texture> AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set) = 0;
	};

} // namespace BHive