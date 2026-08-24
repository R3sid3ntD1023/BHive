#pragma once

#include "core/Core.h"
#include "core/math/Transform.h"
#include "RenderData.h"

namespace BHive
{
	class Framebuffer;
	class UniformBuffer;
	class Shader;

	class ShadowRenderer
	{
	public:
		static constexpr uint32_t sMaxLights = 32;

	public:
		void Init(uint32_t cascaded_levels = 5);

		void BeginRecording();
		void EndRecording();

		void Render(const SubMeshSubmissions &data);

		void SubmitDirectionalLight(const FShadowCascadedCreateInfo &info);
		void SubmitSpotLight(const FShadowFrustumCreateInfo &info);
		void SubmitPointLight(const FShadowCubeCreateInfo &info);

		void BindShadowMaps(uint32_t *bindings);

	private:
		Ref<struct FShadowRenderData> mShadowRenderData;
	};
} // namespace BHive