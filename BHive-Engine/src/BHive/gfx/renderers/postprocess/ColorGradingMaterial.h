#pragma once

#include "PostProcessMaterial.h"
#include "core/math/Math.h"
#include "gfx/material/Material.h"

namespace BHive
{
	class BHIVE_API ColorGradingMaterial : public PostProcessMaterial
	{
	public:
		void Init(const glm::uvec2 &size) override;

		void Resize(const glm::uvec2 &size) override;

		TexturePtr AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set) override;

		struct FParams
		{
			glm::vec3 Lift = {-0.02f, -0.02f, -0.01f}; // shadows
			glm::vec3 Gamma = {0.95f, 0.97f, 1.00f};   // midtones
			glm::vec3 Gain = {1.05f, 1.03f, 1.00f};	   // highlights
			float Saturation = 1.10f;
		} Params;

	private:
		FramebufferPtr mFramebuffer;
		MaterialPtr mMaterial;
		PipelinePtr mPipeline;
	};

	REFLECT_EXTERN(ColorGradingMaterial::FParams);

} // namespace BHive