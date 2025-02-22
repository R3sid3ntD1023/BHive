#pragma once

#include "core/Core.h"
#include "math/Transform.h"
#include "renderers/Lights.h"

namespace BHive
{
	struct PointLight;
	struct IndirectRenderable;
	class ShaderInstance;

	struct ObjectData
	{
		Ref<ShaderInstance> ShaderInstance;
		Ref<IndirectRenderable> Renderable;
		FTransform Transform;

		bool Instanced = false;
		uint32_t Instances = 0;
		glm::mat4 *InstanceTransforms;
	};

	struct LightData
	{
		FTransform Transform;
		PointLight PointLight;
	};

	class UniverseRenderPipeline
	{
	public:
		UniverseRenderPipeline();

		void SubmitLight(const LightData &light);

		void SubmitObject(const ObjectData &data);

		void Begin();

		void End();

		static UniverseRenderPipeline &GetPipeline();

	private:
		std::vector<ObjectData> mObjects;
		std::vector<LightData> mLights;
	};
} // namespace BHive