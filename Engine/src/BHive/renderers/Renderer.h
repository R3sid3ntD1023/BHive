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

	struct BHIVE_API Renderer
	{
		struct BHIVE_API Statitics
		{
			uint32_t DrawCalls;
			uint32_t InstanceCount;
		};

		static void Init();
		static void Shutdown();

		static void Begin();
		static void SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view);

		static void SubmitDirectionalLight(const glm::vec3 &direction, const DirectionalLight &light);
		static void SubmitPointLight(const glm::vec3 &position, const PointLight &light);
		static void SubmitSpotLight(const glm::vec3 &direction, const glm::vec3 &position, const SpotLight &light);
		static void SubmitLight(
			const glm::vec3 &direction, const glm::vec3 &position, const FColor &color, float brightness, float radius,
			float cutoff, float outercutoff, ELightType type);

		static void End();

		static Ref<Texture> GetWhiteTexture();
		static Ref<Texture> GetBlackTexture();

		static void ResetStats();
		static Statitics &GetStats() { return sStats; }
		static const Frustum &GetFrustum();

	private:
		struct RenderData;
		static RenderData *sData;
		static inline Statitics sStats;
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
