#pragma once

#include "gfx/material/Material.h"
#include "PostProcessMaterial.h"

namespace BHive
{
	class BHIVE_API AcesMaterial : public PostProcessMaterial
	{
	public:
		Ref<Texture> AddToGraph(Graph &graph, const FPostProcessTextureSet &set) override;

		void Init(const glm::uvec2 &size) override;

		void Resize(const glm::uvec2 &size) override;

	private:
		FramebufferPtr mFramebuffer;

		Material mMaterial;

		PipelinePtr mPipeline;
	};
} // namespace BHive