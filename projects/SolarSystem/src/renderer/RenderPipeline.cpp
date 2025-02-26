#include "RenderPipeline.h"

namespace BHive
{
	RenderPipeline::RenderPipeline()
	{
		mPipelineData.Objects.reserve(512);
		mPipelineData.Lights[ELightType::Point].reserve(512);
		mPipelineData.Lights[ELightType::Directional].reserve(512);
		mPipelineData.Lights[ELightType::SpotLight].reserve(512);
	}

	void RenderPipeline::SubmitMesh(
		const Ref<IndirectRenderable> &mesh, const FTransform &transform, const Ref<ShaderInstance> &shaderInstance,
		uint32_t instances, const glm::mat4 *transforms)
	{
		mPipelineData.Objects.emplace_back(ObjectData{transform, shaderInstance, mesh, instances, transforms});
	}

	void RenderPipeline::SubmitLight(const Light &light, const FTransform &transform)
	{
		mPipelineData.Lights.at(light.GetLightType()).emplace_back(LightData{transform, light});
	}

	void RenderPipeline::Begin(const glm::mat4 &projection, const glm::mat4 &view)
	{
		mPipelineData.Objects.clear();
		mPipelineData.Lights[ELightType::Point].clear();
		mPipelineData.Lights[ELightType::Directional].clear();
		mPipelineData.Lights[ELightType::SpotLight].clear();

		Renderer::Begin(projection, view);
		mPipelineData.CameraTransform = view;
		mPipelineData.CameraFrustum = Frustum(projection, view);
	}

	void RenderPipeline::End()
	{
		SortObjects();

		OnPipelineEnd();

		Renderer::End();
	}

	void RenderPipeline::SortObjects()
	{
		static auto sorter = [this](const ObjectData &a, const ObjectData &b)
		{
			const auto &A = a.Transform.get_translation();
			const auto &B = b.Transform.get_translation();
			const auto &C = mPipelineData.CameraTransform.get_translation();

			const auto distA = glm::distance(A, C);
			const auto distB = glm::distance(B, C);

			return distA < distB;
		};

		auto &objects = mPipelineData.Objects;
		std::sort(objects.begin(), objects.end(), sorter);
	}
} // namespace BHive