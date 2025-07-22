#pragma once

#include "core/Core.h"
#include "core/math/Transform.h"
#include "renderers/Lights.h"

namespace BHive
{
	class Framebuffer;
	class UniformBuffer;
	class Shader;

	class ShadowRenderer
	{
	public:
		static void Init(uint32_t max_lights, uint32_t cascaded_levels = 5);

		static void Begin();
		static void End();

		static void BeginShadowPass();
		static void EndShadowPass();

		static void BeginSpotShadowPass();
		static void EndSpotShadowPass();

		static void BeginPointShadowPass();
		static void EndPointShadowPass();

		static void
		SubmitDirectionalLight(const glm::vec3 &direction, const glm::mat4 &camera_proj, const glm::mat4 &camera_view);
		static void SubmitSpotLight(const glm::vec3 &direction, const glm::vec3 &position, float radius);
		static void SubmitPointLight(const glm::vec3 &position, float radius);

		static Ref<Framebuffer> GetShadowFBO();
		static Ref<Framebuffer> GetSpotShadowFBO();
		static Ref<Framebuffer> GetPointShadowFBO();
	};
} // namespace BHive