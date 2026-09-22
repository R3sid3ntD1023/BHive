#pragma once

#include "RenderData.h"
#include "core/Core.h"
#include "gfx/rendergraph/Pass.h"

namespace BHive
{
	class SceneRenderer;
	struct RenderBatch;

	class ShadowRenderer
	{
	public:
		static constexpr uint32_t sMaxLights = 32;

	public:
		void Init();

		void BeginRecording();
		void EndRecording(FPass &pass, const RenderBatch &batch);
		void SetLightBuffer(BufferPtr lightBuffer);
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

	private:
		PipelinePtr mPipeline;
		Ref<struct FShadowRenderData> mShadowRenderData;

		BufferPtr mObjectBuffer;
		BufferPtr mIndirectBuffer;
		BufferPtr mVisibilityBuffer;

		ResourceSetPtr mGlobalSet;
		ResourceSetPtr mObjectSet;

		MaterialPtr mCullingMaterial;
	};
} // namespace BHive