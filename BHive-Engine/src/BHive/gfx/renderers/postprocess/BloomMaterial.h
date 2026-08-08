#pragma once

#include "PostProcessMaterial.h"
#include "core/math/Math.h"
#include "gfx/material/Material.h"

namespace BHive
{
	class Framebuffer;

	class BHIVE_API BloomMaterial : public PostProcessMaterial
	{
	public:
		BloomMaterial();

		Ref<Texture> AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input) override;

		void OnResize(const glm::uvec2 &size, PostProcessAllocator &allocator) override;

		const char *GetName() const override { return "Bloom"; }

		struct FBloomParams
		{
			float Threshold{1.0};
			float Radius{0.0001f};
			float Strength{1.0f};
			float Exposure{1.0f};
		} Params;

	private:
		std::array<Ref<Framebuffer>, 2> mFramebuffers;
		std::array<Scope<Material>, 4> mMaterials;
	};
} // namespace BHive