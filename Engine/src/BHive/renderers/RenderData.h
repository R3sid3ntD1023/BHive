#pragma once

#include "core/Core.h"
#include "mesh/MeshData.h"
#include "material/MaterialTable.h"
#include "core/math/Transform.h"

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

	struct FObjectInfo
	{
		FTransform Transform;

		int32_t EntityID = -1;
	};

	struct FMeshRenderData
	{
		enum Type
		{
			Static,
			Skeletal
		};

		FObjectInfo ObjectInfo;

		Ref<VertexArray> VertexArray;

		FSubMesh SubMesh;

		Ref<FInstanceInfo> InstanceInfo;

		virtual ~FMeshRenderData() = default;

		virtual Type GetRenderDataType() const = 0;
	};

	struct FStaticMeshRenderData : public FMeshRenderData
	{
		Type GetRenderDataType() const override { return Type::Static; }
	};

	struct FSkeletalMeshRenderData : public FMeshRenderData
	{
		Ref<FBoneInfo> BoneInfo;

		Type GetRenderDataType() const override { return Type::Skeletal; }
	};

	struct FDirectionalLightCreateInfo
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Direction;
	};

	struct FPointLightCreateInfo
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Position;
		float Radius;
	};

	struct FSpotLightCreateInfo
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Position;
		alignas(16) glm::vec3 Direction;
		float Radius;
		float InnerCutoff;
		float OuterCutoff;
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

	class BaseMesh;

	struct FMeshInfo
	{
		Ref<BaseMesh> Mesh;

		MaterialTable Materials;

		FObjectInfo ObjectInfo;

		Ref<FInstanceInfo> InstanceInfo;

		Ref<FBoneInfo> BoneInfo;
	};

} // namespace BHive