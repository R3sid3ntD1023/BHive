#include "RenderPipeline.h"
#include "renderers/Renderer.h"
#include "indirect_mesh/IndirectMesh.h"
#include "ShaderInstance.h"

namespace BHive
{
	UniverseRenderPipeline::UniverseRenderPipeline()
	{
		mLights.reserve(50);
		mObjects.reserve(50);
	}

	void UniverseRenderPipeline::SubmitLight(const LightData &light)
	{
		mLights.emplace_back(light);
	}

	void UniverseRenderPipeline::SubmitObject(const ObjectData &data)
	{
		mObjects.emplace_back(data);
	}

	void UniverseRenderPipeline::Begin()
	{
		mLights.clear();
		mObjects.clear();
	}

	void UniverseRenderPipeline::End()
	{
		for (auto &light : mLights)
		{
			Renderer::SubmitPointLight(light.Transform.get_translation(), light.PointLight);
		}

		for (auto &object : mObjects)
		{
			if (!object.Renderable || !object.ShaderInstance)
				continue;

			object.ShaderInstance->Bind();

			if (!object.Instanced)
			{
				object.Renderable->Draw(object.Transform);
			}
			else
			{
				object.Renderable->Draw(object.Transform, object.InstanceTransforms);
			}
		}
	}

	UniverseRenderPipeline &UniverseRenderPipeline::GetPipeline()
	{
		static UniverseRenderPipeline pipeline;
		return pipeline;
	}
} // namespace BHive