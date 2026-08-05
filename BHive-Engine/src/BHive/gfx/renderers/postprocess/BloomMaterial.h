#pragma once

#include "PostProcessMaterial.h"
#include "core/math/Math.h"

namespace BHive
{
	class Framebuffer;

	class BHIVE_API BloomMaterial : public PostProcessMaterial
	{
	public:
		BloomMaterial() = default;

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
	};
} // namespace BHive