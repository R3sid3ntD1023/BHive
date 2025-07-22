#pragma once

#include "core/Core.h"
#include "LineRenderer.h"
#include "QuadRenderer.h"
#include "MeshRenderer.h"
#include "gfx/Camera.h"
#include "Lights.h"
#include "core/math/Frustum.h"

namespace BHive
{

	class Texture;
	class Material;
	class VertexArray;
	class Shader;

	struct Renderer
	{
		struct BHIVE Statitics
		{
			uint32_t DrawCalls;
			uint32_t InstanceCount;
		};

		static BHIVE void Init();
		static BHIVE void Shutdown();

		static BHIVE void Begin();
		static void SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view);

		static BHIVE void SubmitDirectionalLight(const glm::vec3 &direction, const DirectionalLight &light);
		static BHIVE void SubmitPointLight(const glm::vec3 &position, const PointLight &light);
		static BHIVE void SubmitSpotLight(const glm::vec3 &direction, const glm::vec3 &position, const SpotLight &light);
		static BHIVE void SubmitLight(
			const glm::vec3 &direction, const glm::vec3 &position, const FColor &color, float brightness, float radius,
			float cutoff, float outercutoff, ELightType type);

		static BHIVE void End();

		static BHIVE Ref<Texture> GetWhiteTexture();
		static BHIVE Ref<Texture> GetBlackTexture();

		static void ResetStats();
		static BHIVE Statitics &GetStats() { return sStats; }
		static const Frustum &GetFrustum();

	private:
		struct RenderData;
		static RenderData *sData;
		static Statitics sStats;
	};

	class UniformBuffer;

	struct CameraBuffer
	{

	public:
		struct FCameraData
		{
			glm::mat4 Projection{1.0f};
			glm::mat4 View{1.0f};
			glm::vec4 NearFar;
			glm::vec4 CameraPosition;
			Frustum ViewFrustum;
		};

		CameraBuffer();

		void Submit(const glm::mat4 &proj, const glm::mat4 &view);

		const FCameraData &GetCameraData() { return mData; }

		static CameraBuffer &Get();

	private:
		Ref<UniformBuffer> mBuffer;
		FCameraData mData;
	};
} // namespace BHive
