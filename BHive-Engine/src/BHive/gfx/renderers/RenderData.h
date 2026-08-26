#pragma once

#include "core/Core.h"
#include "gfx/mesh/MeshData.h"
#include "gfx/material/MaterialTable.h"
#include "core/math/Transform.h"
#include "gfx/Color.h"

namespace BHive
{
	class VertexArray;
	class SkeletalPose;
	class BaseMesh;

	struct ContextHandle
	{
		uint32_t ContextIndex = -1;
		uint32_t Generation = -1;
	};

	struct FMeshSubmissionRequest
	{
		Ref<BaseMesh> Mesh;

		MaterialTable Materials;

		FTransform Transform;

		int32_t EntityID = -1;

		std::vector<glm::mat4> InstanceTransforms;

		std::vector<glm::mat4> BoneTransforms;
	};

	struct FMeshSubmissionContext
	{
		Ref<VertexArray> VAO;

		FTransform Transform;

		int32_t EntityID = -1;

		std::vector<glm::mat4> InstanceTransforms;

		std::vector<glm::mat4> BoneTransforms;

		bool Active = true;

		uint32_t Generation = 0;
	};

	struct FSubMeshSubmission
	{
		AABB BoundingBox{};

		FSubMesh SubMesh;

		uint32_t MeshIndex = 0;

		ResourceHandle MaterialHandle;

		// opaque/transparent, cast_shadows
		std::bitset<2> BitFlags;

		ContextHandle Context;
	};

	using SubMeshSubmissions = std::vector<FSubMeshSubmission>;

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

} // namespace BHive