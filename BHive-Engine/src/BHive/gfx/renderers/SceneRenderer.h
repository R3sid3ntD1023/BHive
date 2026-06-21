#pragma once

#include "core/Core.h"
#include "RenderData.h"
#include "LightCasters.h"
#include "postprocess/PostProcessStack.h"


namespace BHive
{
	class Camera;
	class Framebuffer;
	class Shader;
	class Texture;
	class Texture2D;
	class PQuad;

	struct FTransform;
	class BaseMesh;
	class SkeletalMesh;
	class StaticMesh;
	class SkeletalPose;
	struct MaterialTable;

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
		using Command = std::function<void()>;
		using Commands = std::stack<Command>;

		SceneRenderer() = default;

		virtual ~SceneRenderer() = default;

		virtual void Init(const glm::uvec2 &size);

		void Begin(const Camera *camera, const FTransform &view);

		virtual void End();

		void Submit(const DirectionalLight & light);

		void Submit(const PointLight &light);

		void Submit(const SpotLight &light);

		void Submit(const FMeshInfo &info);

		void SubmitCommand(const Command &cmd);

		void Resize(const glm::uvec2 &size);

		Ref<Texture> GetColorAttachment(uint32_t index = 0) const;

		Ref<Texture> GetDepthAttachment() const;

		Ref<Framebuffer> GetFramebuffer() const { return mFinalFramebuffer; }

		FRenderSettings &GetRenderSettings() { return mRenderSettings; }

		const FRenderSettings &GetRenderSettings() const { return mRenderSettings; }

		void RenderToScreen();

		void AddPostProcessMaterial(const Ref<PostProcessMaterial> & mat);

		void RemovePostProcessMaterial(const std::string& name);

		void ClearPostProcessEffects();

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

		glm::uvec2 mSize{0, 0};

		PostProcessAllocator mPostProcessAllocator;
		PostProcessStack mPostProcessStack;
	};
} // namespace BHive