#pragma once

#include "RenderData.h"
#include "core/Core.h"
#include "gfx/Framebuffer.h"
#include "gfx/rendergraph/Pass.h"

namespace BHive
{
	class SceneRenderer;
	struct RenderBatch;

	class ShadowRenderer
	{
	public:
		static constexpr uint32_t sMaxLights = 32;
		static constexpr uint32_t sCascadeCount = 5;

	public:
		void Init();

		void BeginRecording();
		void EndRecording(FPass &pass, const RenderBatch &batch);
		void SetBoneBuffer(BufferPtr boneBuffer);

		void SubmitDirectionalLight(const FShadowCascadedCreateInfo &info);
		void SubmitSpotLight(const FShadowFrustumCreateInfo &info);
		void SubmitPointLight(const FShadowCubeCreateInfo &info);

		BufferPtr GetBuffer();

		TexturePtr GetDirShadowMap();
		TexturePtr GetPointShadowMap();
		TexturePtr GetSpotShadowMap();

	private:
		void InitializeBuffers();
		void InitializeSets();

		glm::mat4 GetLightMatrix(const Frustum frustum, const glm::vec3 &lightDir, const float nearPlane, const float farPlane);

		std::vector<glm::mat4> GetLightMatrices(const Frustum &frustum, const glm::vec3 &lightDir, const float nearPlane, const float farPlane, std::vector<float> cascadedLevels);

	private:
		struct LightDirections
		{
			glm::vec3 normal;
			glm::vec3 up;
		};

		struct alignas(16) CascadeShadow
		{
			glm::mat4 ViewProjection{1.0f};
			glm::vec4 SplitData{0.0f};
			Frustum Frustum{};
		};

		struct alignas(16) DirectionalShadowData
		{
			glm::vec4 Direction;
			CascadeShadow Cascades[sCascadeCount];
		};

		struct alignas(16) PointLightShadowData
		{
			glm::vec4 Position;
			glm::vec4 ShadowNearFar;
			glm::mat4 ShadowViewProjections[6];
			Frustum Frustums[6];
		};

		struct alignas(16) SpotLightShadowData
		{
			glm::mat4 ViewProjection{1.0f};
			Frustum Frustum{};
		};

		struct alignas(16) ShadowData
		{
			glm::uvec4 NumShadowMaps = {0, 0, 0, 0};
			std::array<DirectionalShadowData, sMaxLights> DirProjections = {};
			std::array<PointLightShadowData, sMaxLights> PointShadowInfos = {};
			std::array<SpotLightShadowData, sMaxLights> SpotProjections = {};
		};

		PipelinePtr mPipeline;
		PipelinePtr mDirectionalPipeline;
		std::array<MaterialPtr, 3> mShadowMaterials;
		std::array<FramebufferPtr, 3> mShadowFramebuffers;
		ShadowData mShadowData;
		BufferPtr mShadowBuffer;

		BufferPtr mObjectBuffer;
		BufferPtr mIndirectBuffer;
		BufferPtr mVisibilityBuffer;

		ResourceSetPtr mObjectSet;
		ResourceSetPtr mShadowSet;

		MaterialPtr mCullingMaterial;
	};
} // namespace BHive