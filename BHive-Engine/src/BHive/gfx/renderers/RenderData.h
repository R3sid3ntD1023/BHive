#pragma once

#include "core/Core.h"
#include "gfx/mesh/MeshData.h"
#include "gfx/material/MaterialTable.h"
#include "core/math/Transform.h"
#include "gfx/Color.h"

#define MAX_LIGHTS 32

namespace BHive
{
	class VertexArray;
	class SkeletalPose;

	struct FInstanceInfo
	{
		std::vector<glm::mat4> Transforms;
	};

	struct FBoneInfo
	{
		std::vector<glm::mat4> Bones;
	};

	struct FMeshRenderData
	{
		enum Type
		{
			Static,
			Skeletal,
			Billboard
		};

		FTransform Transform;

		int32_t EntityID = -1;

		virtual ~FMeshRenderData() = default;

		virtual Type GetRenderDataType() const = 0;
	};

	struct FStaticMeshRenderData : public FMeshRenderData
	{
		Ref<VertexArray> VertexArray;

		FSubMesh SubMesh;

		FInstanceInfo Instances;

		Type GetRenderDataType() const override { return Type::Static; }
	};

	struct FSkeletalMeshRenderData : public FStaticMeshRenderData
	{
		FBoneInfo Bones;

		Type GetRenderDataType() const override { return Type::Skeletal; }
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

	using FMeshRenderDatas = std::vector<Ref<FMeshRenderData>>;

	class BaseMesh;

	struct FMeshInfo
	{
		Ref<BaseMesh> Mesh;

		MaterialTable Materials;

		FTransform Transform;

		int32_t EntityID = -1;

		FInstanceInfo Instances;

		FBoneInfo Bones;
	};

} // namespace BHive