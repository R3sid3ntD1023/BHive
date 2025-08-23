#pragma once

#include "core/Core.h"
#include "Renderer.h"
#include "PMREMGenerator.h"
#include "RenderData.h"

namespace BHive
{
	class Camera;
	class Framebuffer;
	class Shader;
	class Texture;
	class Texture2D;
	class PQuad;
	class Bloom;
	class PostProcessor;
	struct FTransform;
	class PMREMGenerator;
	class BaseMesh;
	class SkeletalMesh;
	class StaticMesh;
	class SkeletalPose;
	class MaterialTable;

	/**
	 * @brief The SceneRenderer class is responsible for rendering the scene.
	 * It manages the rendering process, including setting up the camera and framebuffer.
	 */

	struct FRenderSettings
	{
	};

	class BHIVE_API SceneRenderer
	{
		FRenderSettings mRenderSettings; // Render settings for the scene renderer

	public:
		SceneRenderer() = default;

		void Initialize(uint32_t width, uint32_t height);

		void Begin(const Camera *camera, const FTransform &view);

		void SetEnvironmentMap(const Ref<Texture2D> &environment);

		void End();

		void AddPostProcessingEffect(const Ref<PostProcessor> &processor);

		void SubmitLight(const FDirectionalLightCreateInfo &info);

		void SubmitLight(const FPointLightCreateInfo &info);

		void SubmitLight(const FSpotLightCreateInfo &info);

		void SubmitMesh(const Ref<StaticMesh> &mesh, const glm::mat4 &transform = {1.0f}, const glm::mat4 *instances = nullptr, size_t instanceCount = 0);

		void SubmitMesh(const Ref<SkeletalMesh> &mesh, const SkeletalPose &pose, const glm::mat4 &transform = {1.0f}, const glm::mat4 *instances = nullptr, size_t instanceCount = 0);

		void SubmitMesh(const Ref<StaticMesh> &mesh, const MaterialTable &materials, const glm::mat4 &transform = {1.0f}, const glm::mat4 *instances = nullptr, size_t instanceCount = 0);

		void SubmitMesh(
			const Ref<SkeletalMesh> &mesh, const MaterialTable &materials, const SkeletalPose &pose, const glm::mat4 &transform = {1.0f}, const glm::mat4 *instances = nullptr,
			size_t instanceCount = 0);

		void SubmitCommand(const std::function<void()> cmd);

		void Resize(uint32_t width, uint32_t height);

		const Ref<Texture> &GetColorAttachment(uint32_t index = 0) const;

		const Ref<Texture> &GetDepthAttachment() const;

		const Ref<Texture2D> &GetEnvironmentMap() const;

		const Ref<Framebuffer> &GetFramebuffer() const { return mFinalFramebuffer; }

		void RenderToScreen();

		glm::uvec2 GetSize() const;

	private:
		bool IsMeshCulled(const Ref<BaseMesh> &mesh, const glm::mat4 &transform);

		float GetDistanceToCamera(const FTransform &transform);

	private:
		Ref<Framebuffer> mFramebuffer;
		Ref<Framebuffer> mFinalFramebuffer; // Final framebuffer for post-processing effects
		Ref<PQuad> mQuad;
		Ref<Shader> mQuadShader;	  // Shader used for rendering the quad
		glm::uvec2 mRenderSize{0, 0}; // Size of the renderer

		// Post-processing effects
		std::vector<Ref<PostProcessor>> mPostProcessingEffects; // effects for post-processing
		Ref<struct FSceneRenderData> mSceneRenderData;

		std::stack<std::function<void()>> mCommands;

		static inline PMREMGenerator EnvironmentMapGenerator;
		static inline Ref<Texture2D> sEnvironmentMap = nullptr; // Static environment map

		REFLECTABLE()
	};

	REFLECT_EXTERN(SceneRenderer)
	REFLECT_EXTERN(FRenderSettings)
} // namespace BHive