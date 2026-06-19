#pragma once

#include "PostProcessMaterial.h"
#include "core/math/Math.h"

namespace BHive
{
	class BHIVE_API BloomMaterial : public PostProcessMaterial
	{
	public:
		BloomMaterial();

		Ref<Texture> AddToGraph(RenderGraph &graph, const Ref<Texture> &input) override;

		void CreateResizableObjects(const glm::uvec2 &size) override;

		const char *GetName() const override { return "Bloom"; }

		struct FBloomParams
		{
			glm::vec4 Threshold{0.2126, 0.7152, 0.0722, 1.0};
			float Radius{0.0001f};
		} Params;

	private:
		static uint32_t ComputeMipCount(glm::uvec2 size);

	private:
		Ref<Texture2D> mBloomTex;
		Ref<Texture2D> mOutputTex;
		uint32_t mMipCount{5};
	};
}