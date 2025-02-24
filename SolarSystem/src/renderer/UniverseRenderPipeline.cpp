#include "indirect_mesh/IndirectMesh.h"
#include "mesh/IRenderableAsset.h"
#include "renderers/Renderer.h"
#include "ShaderInstance.h"
#include "UniverseRenderPipeline.h"
#include "volumes/SphereVolume.h"

namespace BHive
{

	void UniverseRenderPipeline::OnPipelineEnd()
	{
		RenderPipeline::OnPipelineEnd();

		for (auto &light : mPipelineData.Lights[ELightType::Point])
		{
			Renderer::SubmitPointLight(light.Transform.get_translation(), static_cast<const PointLight &>(light.Light));
		}

		auto &objects = mPipelineData.Objects;

		for (auto &object : objects)
		{
			auto indirect_mesh = object.Mesh;
			if (!indirect_mesh || !object.ShaderInstance)
				continue;

			object.ShaderInstance->Bind();

			const auto &mesh = indirect_mesh->GetRenderable();
			const auto &bounds = mesh->GetBoundingBox();
			FSphereVolume volume = GenerateSphereFromAABB(bounds);
			bool is_in_frustum = volume.InFrustum(mPipelineData.CameraFrustum, object.Transform);
			if (!is_in_frustum)
				continue;

			if (!object.Instances)
			{
				indirect_mesh->Draw(object.Transform);
			}
			else
			{
				indirect_mesh->Draw(object.Transform, object.InstanceTransforms);
			}

			Renderer::GetStats().DrawCalls++;
		}
	}

} // namespace BHive