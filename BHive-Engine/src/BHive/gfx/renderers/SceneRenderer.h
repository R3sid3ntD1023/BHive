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

	struct FTransform;
	class BaseMesh;
	class SkeletalMesh;
	class StaticMesh;
	class SkeletalPose;
	struct MaterialTable;
	class Renderer;
	class GeneralBuffer;

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
		struct FDrawRange
		{
			uint32_t First = 0;
			uint32_t Count = 0;
		};

	public:
		SceneRenderer() = default;

		virtual ~SceneRenderer() = default;

		virtual void Init(const glm::uvec2 &size);

		void Begin(const Camera *camera, const FTransform &view);

		virtual void End();

		void Submit(const DirectionalLight &light);

		void Submit(const PointLight &light);

		void Submit(const SpotLight &light);

		void Submit(const FMeshInfo &info);

		void Resize(const glm::uvec2 &size);

		Ref<Texture> GetOutput() const { return mOutputTexture; }

		FRenderSettings &GetRenderSettings() { return mRenderSettings; }

		const FRenderSettings &GetRenderSettings() const { return mRenderSettings; }

		void RenderToScreen();

		void AddPostProcessMaterial(const Ref<PostProcessMaterial> &mat);

		void RemovePostProcessMaterial(const std::string &name);

		void ClearPostProcessEffects();

		const glm::uvec2 &GetSize() const { return mSize; }

	private:
		bool IsMeshCulled(const Ref<BaseMesh> &mesh, const glm::mat4 &transform);

		float GetDistanceToCamera(const FTransform &transform);

	private:
		FRenderSettings mRenderSettings; // Render settings for the scene renderer

		Ref<Framebuffer> mFramebuffer;

		Ref<Texture> mOutputTexture;

		FView mView;

		Ref<struct FSceneRenderData> mSceneRenderData;

		Ref<GeneralBuffer> mIndirectDrawBuffer;

		glm::uvec2 mSize{0, 0};

		// PostProcessAllocator mPostProcessAllocator;
		// PostProcessStack mPostProcessStack;
	};
} // namespace BHive