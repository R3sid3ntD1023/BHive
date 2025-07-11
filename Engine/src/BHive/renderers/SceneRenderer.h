#pragma once

#include "core/Core.h"
#include "Renderer.h"
#include "PMREMGenerator.h"

namespace BHive
{
	class Camera;
	class Framebuffer;
	class Shader;
	class Texture;
	class PQuad;
	class Bloom;
	class PostProcessor;
	struct FTransform;
	class PMREMGenerator;

	/**
	 * @brief The SceneRenderer class is responsible for rendering the scene.
	 * It manages the rendering process, including setting up the camera and framebuffer.
	 */

	struct SceneRendererFlags
	{
		enum Flags : uint16_t
		{
			None = 0,			 // No flags set
			RenderQuad = 1 << 0, // Flag to determine if the quad should be rendered
			Bloom = 1 << 1,		 // Flag to enable bloom post-processing
		};
	};

	class SceneRenderer
	{
	public:
		SceneRenderer() = default;

		void Initialize(uint32_t width, uint32_t height, uint16_t flags = 0);

		void Begin(const Camera *camera, const FTransform &view);

		void SetEnvironmentMap(const Ref<Texture> &environment);

		void End();

		void AddPostProcessingEffect(const Ref<PostProcessor> &processor);

		void Resize(uint32_t width, uint32_t height);

		const Ref<Texture> &GetColorAttachment(uint32_t index = 0) const;

		Ref<Bloom> GetBloom() const { return mBloom; }

	private:
		Ref<Framebuffer> mFramebuffer;
		Ref<Framebuffer> mFinalFramebuffer; // Final framebuffer for post-processing effects
		Ref<PQuad> mQuad;
		Ref<Shader> mQuadShader;						   // Shader used for rendering the quad
		uint16_t mFlags = SceneRendererFlags::Flags::None; // Flag to determine if the quad should be rendered

		// Post-processing effects
		Ref<Bloom> mBloom;										// Bloom effect for post-processing
		std::vector<Ref<PostProcessor>> mPostProcessingEffects; // effects for post-processing

		static inline PMREMGenerator EnvironmentMapGenerator;
	};
} // namespace BHive