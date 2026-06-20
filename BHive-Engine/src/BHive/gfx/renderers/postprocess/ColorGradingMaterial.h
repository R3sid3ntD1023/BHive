#pragma once

#include "PostProcessMaterial.h"
#include "core/math/Math.h"

namespace BHive
{
	class BHIVE_API ColorGradingMaterial : public PostProcessMaterial
	{
	public:
		ColorGradingMaterial();

		Ref<Texture> AddToGraph(RenderGraph &graph, const Ref<Texture> &input) override;

		void CreateResizableObjects(const glm::uvec2 &size) override;

		const char *GetName() const override { return "Color Grading"; }

		struct FColorGrading
		{
			glm::vec3 Lift = {-0.02f, -0.02f, -0.01f}; //shadows
			glm::vec3 Gamma = {0.95f, 0.97f, 1.00f}; // midtones
			glm::vec3 Gain = {1.05f, 1.03f, 1.00f};//highlights
			float Saturation = 1.10f;
		} Params;

	private:
		Ref<Texture2D> mOutputTex;
	};
} // namespace BHive