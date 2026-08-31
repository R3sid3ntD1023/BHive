#pragma once

#include "PostProcessMaterial.h"
#include "gfx/material/Material.h"

namespace BHive
{
	class Texture2D;
	class Framebuffer;

	class BHIVE_API AcesMaterial : public PostProcessMaterial
	{
	public:
		AcesMaterial();

		TexturePtr AddToGraph(RenderGraph &graph, const FPostProcessTextureSet &set) override;

		void Init(const glm::uvec2 &size) override;

	private:
		Ref<Framebuffer> mFramebuffer;

		Scope<Material> mMaterial;
	};
} // namespace BHive