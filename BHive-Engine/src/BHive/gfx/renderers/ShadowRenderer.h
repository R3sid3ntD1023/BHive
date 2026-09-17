#pragma once

#include "RenderData.h"
#include "core/Core.h"
#include "gfx/rendergraph/Pass.h"

namespace BHive
{
	class SceneRenderer;

	class ShadowRenderer
	{
	public:
		static constexpr uint32_t sMaxLights = 32;

	public:
		void Init(uint32_t cascaded_levels = 5);

		void BeginRecording();
		void EndRecording(FPass &pass, SceneRenderer *sceneRenderer);

		void SubmitDirectionalLight(const FShadowCascadedCreateInfo &info);
		void SubmitSpotLight(const FShadowFrustumCreateInfo &info);
		void SubmitPointLight(const FShadowCubeCreateInfo &info);

		BufferPtr GetBuffer();

		TexturePtr GetDirShadowMap();
		TexturePtr GetPointShadowMap();
		TexturePtr GetSpotShadowMap();

	private:
		PipelinePtr mPipeline;
		Ref<struct FShadowRenderData> mShadowRenderData;
	};
} // namespace BHive