#include "indirect_mesh/IndirectMesh.h"
#include "mesh/IRenderableAsset.h"
#include "renderers/Renderer.h"
#include "RenderPipeline.h"
#include "ShaderInstance.h"
#include "volumes/SphereVolume.h"

namespace BHive
{

	UniverseRenderPipeline::UniverseRenderPipeline()
	{
		mPipelineData.LightData.reserve(50);
		mPipelineData.ObjectData.reserve(50);
	}

	void UniverseRenderPipeline::SubmitLight(const LightData &light)
	{
		mPipelineData.LightData.emplace_back(light);
	}

	void UniverseRenderPipeline::SubmitObject(const ObjectData &data)
	{
		mPipelineData.ObjectData.emplace_back(data);
	}

	void UniverseRenderPipeline::Begin(const glm::mat4 &projection, const glm::mat4 &view)
	{
		mPipelineData.LightData.clear();
		mPipelineData.ObjectData.clear();
		mPipelineData.CameraTransform = view;
		mPipelineData.CameraFrustum = Frustum(projection, view);

		Renderer::Begin(projection, view);
	}

	void UniverseRenderPipeline::End()
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

		for (auto &light : mPipelineData.LightData)
		{
			Renderer::SubmitPointLight(light.Transform.get_translation(), light.PointLight);
		}

		auto &objects = mPipelineData.ObjectData;
		// sort objects based on distance from camera
		std::sort(objects.begin(), objects.end(), sorter);

		for (auto &object : objects)
		{
			auto indirect_mesh = object.Renderable;
			if (!indirect_mesh || !object.ShaderInstance)
				continue;

			object.ShaderInstance->Bind();

			const auto &mesh = indirect_mesh->GetRenderable();
			const auto &bounds = mesh->GetBoundingBox();
			FSphereVolume volume = GenerateSphereFromAABB(bounds);
			bool is_in_frustum = volume.InFrustum(mPipelineData.CameraFrustum, object.Transform);
			if (!is_in_frustum)
				continue;

			if (!object.Instanced)
			{
				indirect_mesh->Draw(object.Transform);
			}
			else
			{
				indirect_mesh->Draw(object.Transform, object.InstanceTransforms);
			}
		}

		Renderer::End();
	}

	UniverseRenderPipeline &UniverseRenderPipeline::GetPipeline()
	{
		static UniverseRenderPipeline pipeline;
		return pipeline;
	}
} // namespace BHive