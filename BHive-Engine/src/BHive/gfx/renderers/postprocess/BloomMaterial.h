#pragma once

#include "PostProcessMaterial.h"
#include "core/math/Math.h"

namespace BHive
{

	class BHIVE_API BloomMaterial : public PostProcessMaterial
	{
		static inline constexpr uint32_t MipCount = 5;

	public:
		TexturePtr AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set) override;

		void Init(const glm::uvec2 &size) override;

		void Resize(const glm::uvec2 &size) override;

		struct FParams
		{
			float Threshold{1.0};
			float Radius{0.0001f};
			float Strength{1.0f};
			float Exposure{1.0f};

		} Params;

	private:
		uint32_t ComputeMipCount(glm::uvec2 size);

		glm::uvec2 GetBloomMipSize(uint32_t mip) const
		{
			ASSERT(mip < mMipSizes.size());
			return mMipSizes[mip];
		}

		void CalculateMipSizes(const glm::uvec2 &size);

	private:
		std::array<FramebufferPtr, 2> mFramebuffers;
		std::array<MaterialPtr, 4> mMaterials;
		std::vector<glm::uvec2> mMipSizes;
		PipelinePtr mPipeline;
	};

	REFLECT_EXTERN(BloomMaterial::FParams);
} // namespace BHive