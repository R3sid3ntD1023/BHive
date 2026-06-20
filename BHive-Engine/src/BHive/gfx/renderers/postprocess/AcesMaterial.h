#pragma once

#include "PostProcessMaterial.h"

namespace BHive
{
	class Texture2D;

	class BHIVE_API AcesMaterial : public PostProcessMaterial
	{
	public:
		AcesMaterial();

		Ref<Texture> AddToGraph(RenderGraph &graph, const Ref<Texture> &input) override;

		void CreateResizableObjects(const glm::uvec2 &size) override;

		const char *GetName() const override { return "Aces"; }

	private:
		void ExecutePass(FComputeBindings& b, const Ref<Texture>& in, const Ref<Texture>& out);

		void OnExecutePass(IRendererContext &ctx, const Ref<Texture>& input);

	private:
		Ref<Texture2D> mOutput;
	};
}