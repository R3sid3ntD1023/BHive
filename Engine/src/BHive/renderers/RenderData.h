#pragma once

#include "core/Core.h"
#include "mesh/MeshData.h"

#define MAX_LIGHTS 32

namespace BHive
{
	class VertexArray;
	class SkeletalPose;

	struct FMeshRenderData
	{
		Ref<VertexArray> VertexArray;
		FSubMesh SubMesh;
		glm::mat4 Transform;

		// instances
		const glm::mat4 *Instances = nullptr;
		size_t InstanceCount = 0;

		virtual ~FMeshRenderData() = default;
	};

	struct FStaticMeshRenderData : public FMeshRenderData
	{
	};

	struct FSkeletalMeshRenderData : public FMeshRenderData
	{
		std::vector<glm::mat4> Bones;
	};

	struct FShadowCascadedCreateInfo
	{
		glm::vec3 LightDirection;

		float LightCascadeFrustumNear;

		glm::vec2 CameraNearFar;

		glm::mat4 CameraProj;

		glm::mat4 InverseCameraView;
	};

	struct FShadowCubeCreateInfo
	{
		glm::vec3 LightPosition;

		glm::vec2 LightNearFar;
	};

	struct FShadowFrustumCreateInfo
	{
		glm::vec3 LightPosition;

		glm::vec3 LightDirection;

		glm::vec3 LightAngleNearFar;
	};

	using FMeshRenderDatas = std::map<float, Ref<FMeshRenderData>>;

} // namespace BHive