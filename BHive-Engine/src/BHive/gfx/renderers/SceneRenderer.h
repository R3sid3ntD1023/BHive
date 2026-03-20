#pragma once

#include "core/Core.h"
#include "PMREMGenerator.h"
#include "RenderData.h"
#include "Renderer.h"

namespace BHive
{
	class Camera;
	class Framebuffer;
	class Shader;
	class Texture;
	class Texture2D;
	class PQuad;

	struct FTransform;
	class PMREMGenerator;
	class BaseMesh;
	class SkeletalMesh;
	class StaticMesh;
	class SkeletalPose;
	class MaterialTable;

	class RenderPass;
	class PostProcessRenderPass;

	/**
	 * @brief The SceneRenderer class is responsible for rendering the scene.
	 * It manages the rendering process, including setting up the camera and framebuffer.
	 */

	struct FRenderSettings
	{
		bool DrawColliders{true};
	};

	class BHIVE_API SceneRenderer
	{

	public:
		using PostProcessPasses = std::vector<Ref<PostProcessRenderPass>>;
		using RenderPasses = std::vector<Ref<RenderPass>>;
		using Command = std::function<void()>;
		using Commands = std::stack<Command>;

		SceneRenderer() = default;

		virtual ~SceneRenderer() = default;

		virtual void Init(const glm::uvec2 &size);

		void Begin(const Camera *camera, const FTransform &view);

		virtual void End();

		void SubmitLight(const FDirectionalLightCreateInfo &info);

		void SubmitLight(const FPointLightCreateInfo &info);

		void SubmitLight(const FSpotLightCreateInfo &info);

		void SubmitMesh(const FMeshInfo &info);

		void SubmitCommand(const Command &cmd);

		void Resize(const glm::uvec2 &size);

		void SetEnvironmentMap(const Ref<Texture2D> &environment);

		const Ref<Texture> &GetColorAttachment(uint32_t index = 0) const;

		const Ref<Texture> &GetDepthAttachment() const;

		const Ref<Texture2D> &GetEnvironmentMap() const;

		const Ref<Framebuffer> &GetFramebuffer() const { return mFinalFramebuffer; }

		FRenderSettings &GetRenderSettings() { return mRenderSettings; }

		const FRenderSettings &GetRenderSettings() const { return mRenderSettings; }

		void RenderToScreen();

		void PushPostProcessRenderPass(const Ref<PostProcessRenderPass> &pass);

		void PushRenderPass(const Ref<RenderPass> &render_pass);

		template <typename T = RenderPass>
		Ref<T> PushPostProcessRenderPass()
			requires(std::is_base_of_v<PostProcessRenderPass, T>)
		{
			auto post_process = CreateRef<T>();
			PushPostProcessRenderPass(post_process);
			return post_process;
		}

		template <typename T = RenderPass>
		Ref<T> PushRenderPass()
			requires(std::is_base_of_v<RenderPass, T>)
		{
			auto pass = CreateRef<T>();
			PushRenderPass(pass);
			return pass;
		}

		PostProcessPasses &GetPostProcessPasses();

		const glm::uvec2 &GetSize() const { return mSize; }

	private:
		bool IsMeshCulled(const Ref<BaseMesh> &mesh, const glm::mat4 &transform);

		float GetDistanceToCamera(const FTransform &transform);

	private:
		FRenderSettings mRenderSettings; // Render settings for the scene renderer

		Ref<Framebuffer> mFramebuffer;
		Ref<Framebuffer> mFinalFramebuffer; // Final framebuffer for post-processing effects
		Ref<PQuad> mQuad;
		Ref<Shader> mQuadShader; // Shader used for rendering the quad

		Ref<struct FSceneRenderData> mSceneRenderData;

		Commands mCommands;
		RenderPasses mRenderPasses;
		PostProcessPasses mPostProcessRenderPasses;

		static inline PMREMGenerator EnvironmentMapGenerator;
		static inline Ref<Texture2D> sEnvironmentMap = nullptr; // Static environment map

		glm::uvec2 mSize{0, 0};
	};
} // namespace BHive