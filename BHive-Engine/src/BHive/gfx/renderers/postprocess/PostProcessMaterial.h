#pragma once

#include "gfx/rendergraph/Graph.h"

namespace BHive
{
	class Graph;

	struct FPostProcessTextureSet
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

		virtual void Resize(const glm::uvec2 &size) = 0;

		virtual Ref<Texture> AddToGraph(Graph &graph, const FPostProcessTextureSet &set) = 0;
	};

} // namespace BHive