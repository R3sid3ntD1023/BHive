#pragma once

#include "EnvironmentSystem.h"
#include "LightCasters.h"
#include "Lights.h"
#include "PMREMGenerator.h"
#include "RenderData.h"
#include "RenderQueue.h"
#include "ShadowRenderer.h"
#include "core/Core.h"
#include "core/math/Frustum.h"
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
		struct ResourceSets
		{
			ResourceSetPtr GlobalSet;			 // 0
			ResourceSetPtr OpaqueObjectSet;		 // 3
			ResourceSetPtr TransparentObjectSet; // 3
		};

		struct SceneView
		{
			FView View;
			Frustum Frustum;
		};

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

		void UpdateMesh(ContextHandle requestHandle, MeshPtr mesh);

		void UpdateTransform(ContextHandle requestHandle, const FTransform &t);

		void UpdateBones(ContextHandle handle, const std::vector<glm::mat4> &bones);

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

		const SceneView &GetSceneView() const { return mSceneView; }

	private:
		void InitPipelines();

		void InitResourceSets();

		void BindResourceSets();

	private:
		FRenderSettings mRenderSettings; // Render settings for the scene renderer

		FramebufferPtr mFramebuffer;

		TexturePtr mOutputTexture;

		std::array<MaterialPtr, 2> mFrustrumOcclusionMaterial;

		MaterialPtr mFrustumMaterial;

		BufferPtr mCameraUBO;

		std::array<BufferPtr, 2> mVisibleBuffer;

		std::array<BufferPtr, 2> mIndirectDrawBuffer;

		std::array<BufferPtr, 2> mInstanceDataBuffer;

		std::array<BufferPtr, 2> mBoneBuffer;

		SceneView mSceneView;

		Ref<struct FRenderQueue> mRenderQueue;

		std::vector<Ref<struct RenderBatch>> mRenderBatches;

		glm::uvec2 mSize{0, 0};

		PostProcessStack mPostProcessStack;

		Lights mLights;

		ShadowRenderer mShadows;

		EnvironmentSystem mEnvironment;

		PipelinePtr mOpaquePipeline;

		PipelinePtr mTransparentPipeline;

		ResourceSets mSceneSets;
	};
} // namespace BHive