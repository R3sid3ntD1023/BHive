#pragma once

#include "PostProcessMaterial.h"
#include "gfx/material/Material.h"

namespace BHive
{
	class BHIVE_API AcesMaterial : public PostProcessMaterial
	{
	public:
		AcesMaterial();

		TexturePtr AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set) override;

		void Init(const glm::uvec2 &size) override;

		void Resize(const glm::uvec2 &size) override;

	private:
		FramebufferPtr mFramebuffer;

		MaterialPtr mMaterial;
	};
} // namespace BHive