#pragma once

#include "PostProcessMaterial.h"

namespace BHive
{
	class Texture2D;

	class BHIVE_API AcesMaterial : public PostProcessMaterial
	{
	public:
		AcesMaterial() = default;

		Ref<Texture> AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input) override;

		const char *GetName() const override { return "Aces"; }

	private:
		void ExecutePass(FComputeBindings& b, const Ref<Texture>& in, const Ref<Texture>& out);

		void OnExecutePass(IRendererContext &ctx, const Ref<Texture>& in, const Ref<Texture>& out);
	};
}