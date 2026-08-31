#pragma once

#include "core/Core.h"
#include "RenderData.h"
#include "LightCasters.h"
#include "postprocess/PostProcessStack.h"
#include "Lights.h"
#include "ShadowRenderer.h"
#include "PMREMGenerator.h"
#include "EnvironmentSystem.h"
#include "RenderQueue.h"

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
	class Query;

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
		SceneRenderer() = default;

		virtual ~SceneRenderer() = default;

		virtual void Init(const glm::uvec2 &size);

		void Begin(const Camera *camera, const glm::mat4 &view);

		virtual void End();

		void SetViewOverride(const FView &view);

		void SetEnvironmentTexture(Texture2DPtr hdr);

		void Submit(const DirectionalLight &light);

		void Submit(const PointLight &light);

		void Submit(const SpotLight &light);

		void SubmitMesh(const FMeshSubmissionRequest &info, ContextHandle &outHandle);

		void SubmitMesh(const FMeshSubmissionRequest &info);

		void UpdateTransform(ContextHandle requestHandle, const FTransform &t);

		void UpdateMesh(ContextHandle requestHandle, MeshPtr mesh);

		void Resize(const glm::uvec2 &size);

		TexturePtr GetOutput() const { return mOutputTexture; }

		FRenderSettings &GetRenderSettings() { return mRenderSettings; }

		const FRenderSettings &GetRenderSettings() const { return mRenderSettings; }

		void RenderToScreen();

		template <typename T>
		T *AddPostProcessMaterial()
		{
			return mPostProcessStack.Emplace<T>();
		}

		void AddPostProcessMaterial(const Ref<PostProcessMaterial> &mat);

		void RemovePostProcessMaterial(const std::string &name);

		void ClearPostProcessEffects();

		const glm::uvec2 &GetSize() const { return mSize; }

		PostProcessStack &GetPostProcessStack() { return mPostProcessStack; }

		const EnvironmentSystem &GetEnvironmentSystem() const { return mEnvironment; }

		const Frustum &GetFrustrum() const { return mFrustum; }

	private:
		float GetDistanceToCamera(const FTransform &transform);

	private:
		FRenderSettings mRenderSettings; // Render settings for the scene renderer

		FramebufferPtr mFramebuffer;

		TexturePtr mOutputTexture;

		std::array<MaterialPtr, 2> mFrustrumOcclusionMaterial;

		MaterialPtr mFrustumMaterial;

		Ref<GeneralBuffer> mFrustumUBO;

		Ref<GeneralBuffer> mCameraUBO;

		std::array<Ref<GeneralBuffer>, 2> mVisibleBuffer;

		std::array<Ref<GeneralBuffer>, 2> mIndirectDrawBuffer;

		std::array<Ref<GeneralBuffer>, 2> mInstanceDataBuffer;

		FView mView;

		Frustum mFrustum;

		Ref<struct FRenderQueue> mRenderQueue;

		std::vector<Ref<struct RenderBatch>> mRenderBatches;

		glm::uvec2 mSize{0, 0};

		PostProcessStack mPostProcessStack;

		Lights mLights;

		ShadowRenderer mShadows;

		EnvironmentSystem mEnvironment;
	};
} // namespace BHive