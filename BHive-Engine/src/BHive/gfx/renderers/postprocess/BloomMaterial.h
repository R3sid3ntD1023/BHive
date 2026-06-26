#pragma once

#include "PostProcessMaterial.h"
#include "core/math/Math.h"

namespace BHive
{
	class BHIVE_API BloomMaterial : public PostProcessMaterial
	{
	public:
		BloomMaterial() = default;

		Ref<Texture> AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input) override;

		const char *GetName() const override { return "Bloom"; }

		struct FBloomParams
		{
			float Threshold{1.0};
			float Radius{0.0001f};
			float Strength{1.0f};
			float Exposure{1.0f};
		} Params;

	private:
		void DoPrefilterSceneColor(IRendererContext &ctx);
		void DoDownSample(IRendererContext &ctx, uint32_t srcMip, uint32_t dstMip);
		void DoUpSample(IRendererContext &ctx, uint32_t mipCount, uint32_t srcMip, uint32_t dstMip);
		void DoComposite(IRendererContext &ctx, uint32_t mipCount);

		Ref<Texture> mInput;
		Ref<Texture> mBloomOutput;
		Ref<Texture> mCompositeOutput;
		PostProcessAllocator *mAllocator = nullptr;
	};
}