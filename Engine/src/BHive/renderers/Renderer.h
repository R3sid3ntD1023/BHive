#pragma once

#include "core/Core.h"
#include "LineRenderer.h"
#include "QuadRenderer.h"
#include "gfx/Camera.h"
#include "Lights.h"

namespace BHive
{
	enum class ELightType : uint32_t
	{
		Directional = 0,
		Point = 1,
		SpotLight = 2
	};

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

		static BHIVE void Begin(const glm::mat4 &projection, const glm::mat4 &view);

		static BHIVE void SubmitDirectionalLight(const glm::vec3 &direction, const DirectionalLight &light);
		static BHIVE void SubmitPointLight(const glm::vec3 &position, const PointLight &light);
		static BHIVE void SubmitSpotLight(const glm::vec3 &direction, const glm::vec3 &position, const SpotLight &light);
		static BHIVE void SubmitLight(
			const glm::vec3 &direction, const glm::vec3 &position, const Color &color, float brightness, float radius,
			float cutoff, float outercutoff, ELightType type);

		static void SubmitTransform(const glm::mat4 &transform);
		static void SubmitSkeletalMesh(const std::vector<glm::mat4> &bone_matrices);

		static BHIVE void End();

		static BHIVE Ref<Texture> GetWhiteTexture();
		static BHIVE Ref<Texture> GetBlackTexture();

		static void ResetStats();
		static BHIVE Statitics &GetStats() { return sStats; }

	private:
		struct RenderData;
		static RenderData *sData;
		static Statitics sStats;
	};
} // namespace BHive
