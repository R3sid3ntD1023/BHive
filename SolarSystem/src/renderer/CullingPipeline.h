#pragma once

#include "core/Core.h"
#include "math/Transform.h"
#include "math/Frustum.h"

namespace BHive
{
	struct IndirectRenderable;
	class Shader;

	class CullingPipeline
	{
	public:
		struct ObjectData
		{
			Ref<IndirectRenderable> Renderable;
			FTransform Transform;

			uint32_t Instances = 0;
			glm::mat4 *InstanceTransforms = nullptr;
		};

	public:
		CullingPipeline();

		void SubmitObject(const ObjectData &object);

		void Begin(const glm::mat4 &proj, const glm::mat4 &view);
		void SetTestFrustum(const Frustum &frustum);

		void End();

		static CullingPipeline &GetPipeline();

	private:
		struct PipelineData
		{
			FTransform CameraTransform;
			std::vector<ObjectData> Objects;
			Frustum TestFrustum;
		};

	private:
		PipelineData mData;
		Ref<Shader> mCullingShader;
	};
} // namespace BHive