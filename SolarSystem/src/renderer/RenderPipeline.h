#pragma once

#include "core/Core.h"
#include "math/Transform.h"
#include "renderers/Lights.h"
#include "renderers/Renderer.h"

namespace BHive
{
	class ShaderInstance;
	class IndirectRenderable;

	class RenderPipeline
	{
	public:
		struct ObjectData
		{
			FTransform Transform{};
			Ref<ShaderInstance> ShaderInstance;
			Ref<IndirectRenderable> Mesh;
			uint32_t Instances{};
			const glm::mat4 *InstanceTransforms = nullptr;
		};

		struct LightData
		{
			FTransform Transform;
			const Light &Light;
		};

		struct FPipelineData
		{
			Frustum CameraFrustum;
			FTransform CameraTransform;
			std::vector<ObjectData> Objects;
			std::unordered_map<ELightType, std::vector<LightData>> Lights;
		};

	public:
		RenderPipeline();

		virtual ~RenderPipeline() = default;

		void SubmitMesh(
			const Ref<IndirectRenderable> &mesh, const FTransform &transform, const Ref<ShaderInstance> &instance = nullptr,
			uint32_t instances = 0, const glm::mat4 *transforms = nullptr);

		void SubmitLight(const Light &light, const FTransform &transform);

		virtual void Begin(const glm::mat4 &projection, const glm::mat4 &view);

		virtual void End();

	protected:
		virtual void OnPipelineEnd() {};

	private:
		void SortObjects();

	protected:
		FPipelineData mPipelineData;
	};

	struct RenderPipelineManager
	{
		void SetCurrentPipeline(RenderPipeline *pipeline) { mCurrentPipeline = pipeline; }

		RenderPipeline *GetCurrentPipeline() { return mCurrentPipeline; }

	private:
		RenderPipeline *mCurrentPipeline = nullptr;
	};

	inline RenderPipelineManager &GetRenderPipelineManager()
	{
		static RenderPipelineManager manager;
		return manager;
	}

} // namespace BHive