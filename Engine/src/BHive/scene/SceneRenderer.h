#pragma once

#include "gfx/Framebuffer.h"
#include "math/Math.h"
#include "mesh/StaticMesh.h"
#include "renderers/Renderer.h"

namespace BHive
{
	class VertexArray;
	class Shader;
	class Texture2D;
	class Texture;
	using MipMaps = std::vector<Ref<Texture2D>>;
	class StaticMesh;
	class SkeletalMesh;
	class SkeletalPose;
	class UniformBuffer;
	class Material;
	class PPlane;

	struct FObjectData
	{
		FTransform mTransform;

		FSubMesh *mMesh = nullptr;

		Ref<VertexArray> mVAO;

		// skeletal mesh
		std::vector<glm::mat4> mJoints;
	};

	struct FObjectMaterialData
	{
		Ref<Material> mMaterial;
		FObjectData mObjectData;
	};

	struct FLightData
	{
		const Light *mLight = nullptr;
		FTransform mTransform;
	};

	using ObjectList = std::vector<FObjectMaterialData>;
	using LightList = std::unordered_map<ELightType, std::vector<FLightData>>;
	using ShadowCasterList = std::vector<FObjectData>;

	struct FSceneData
	{
		glm::mat4 mProjection{1.f}, mView{1.f};

		LightList mLights;

		ObjectList mOpaqueObjects;
		ObjectList mTransparentObjects;
		ShadowCasterList mShadowCasters;

		void clear()
		{
			mLights.clear();
			mOpaqueObjects.clear();
			mTransparentObjects.clear();
			mShadowCasters.clear();
		}
	};

	enum EPostProcessMode : uint32_t
	{
		EPostProcessMode_None,
		EPostProcessMode_ACES
	};

	enum ESceneRendererFlags : uint32_t
	{
		ESceneRendererFlags_None,
		ESceneRendererFlags_NoShadows = BIT(0),
		ESceneRendererFlags_VisualizeColliders = BIT(1),
	};

	struct FRenderSettings
	{
		float mStrength = 1.0f;
		float mFilterRadius = 0.003f;
		glm::vec4 mThreshold = {0.2126, 0.7152, 0.0722, 1.0};

		EPostProcessMode mProcessMode = EPostProcessMode_ACES;

		REFLECTABLE()
	};

	class BHIVE SceneRenderer
	{
	public:
		FRenderSettings mRenderSettings;

		SceneRenderer(const glm::ivec2 &size, uint32_t flags = 0);
		~SceneRenderer();

		void Begin(const Camera &camera, const FTransform &view);
		// void BeginZPass(const glm::mat4& projection, const glm::mat4& view);
		// void EndZPass();
		void End();

		void SubmitLight(const DirectionalLight &light, const FTransform &transform);
		void SubmitLight(const PointLight &light, const FTransform &transform);
		void SubmitLight(const SpotLight &light, const FTransform &transform);

		void
		SubmitStaticMesh(const Ref<StaticMesh> &static_mesh, const FTransform &transform, const Ref<Material> &material);

		void
		SubmitStaticMesh(const Ref<StaticMesh> &static_mesh, const FTransform &transform, const MaterialTable &materials);
		void SubmitSkeletalMesh(
			const Ref<SkeletalMesh> &skeletal_mesh, const FTransform &transform, const std::vector<glm::mat4> &joints,
			const MaterialTable &materials);

		void SubmitRenderQueue(const std::function<void()> &func);

		void Resize(uint32_t width, uint32_t height);

		const glm::ivec2 &GetViewportSize() const { return mViewportSize; }
		const Ref<Framebuffer> &GetFramebuffer() const;
		const Ref<Framebuffer> &GetFinalFramebuffer() const;

		const ESceneRendererFlags &GetFlags() const { return mFlags; }

	private:
		void RenderShadows(bool render_shadows);

	private:
		Ref<Framebuffer> mMultiSampleFramebuffer;
		Ref<Framebuffer> mFramebuffer;
		Ref<Framebuffer> mQuadFramebuffer;
		// Ref<Framebuffer> mZPassFramebuffer;

		Ref<PPlane> mScreenQuad;
		Ref<Shader> mQuadShader;
		glm::ivec2 mViewportSize;

		Ref<Shader> mSkyBoxShader;
		Ref<StaticMesh> mCube;

		std::vector<std::function<void()>> mRenderQueue;
		FSceneData mSceneData;
		TEnumAsByte<ESceneRendererFlags> mFlags;

		std::vector<Ref<class PostProcessor>> mPostProcessors;

		REFLECTABLE()
	};

	REFLECT_EXTERN(SceneRenderer);
	REFLECT_EXTERN(FRenderSettings);

} // namespace BHive