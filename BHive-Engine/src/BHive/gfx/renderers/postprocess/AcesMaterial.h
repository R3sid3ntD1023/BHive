#pragma once

#include "PostProcessMaterial.h"

namespace BHive
{
	class Texture2D;
	class Framebuffer;

	class BHIVE_API AcesMaterial : public PostProcessMaterial
	{
	public:
		AcesMaterial() = default;

		Ref<Texture> AddToGraph(RenderGraph &graph, PostProcessAllocator &allocator, const Ref<Texture> &input) override;

		void OnResize(const glm::uvec2 &size, PostProcessAllocator &allocator) override;

		const char *GetName() const override { return "Aces"; }

	private:
		Ref<Framebuffer> mFramebuffer;
	};
} // namespace BHive