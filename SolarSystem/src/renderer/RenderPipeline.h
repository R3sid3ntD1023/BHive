#pragma once

#include "core/Core.h"
#include "math/Frustum.h"
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

	struct FPipelineData
	{
		Frustum CameraFrustum;
		FTransform CameraTransform;
		std::vector<ObjectData> ObjectData;
		std::vector<LightData> LightData;
	};

	class UniverseRenderPipeline
	{
	public:
		UniverseRenderPipeline();

		void SubmitLight(const LightData &light);

		void SubmitObject(const ObjectData &data);

		void Begin(const glm::mat4 &projection, const glm::mat4 &view);

		void End();

		static UniverseRenderPipeline &GetPipeline();

	private:
		FPipelineData mPipelineData;
	};
} // namespace BHive