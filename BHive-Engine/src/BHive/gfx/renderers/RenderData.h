#pragma once

#include "core/Core.h"
#include "core/math/Transform.h"
#include "gfx/Color.h"
#include "gfx/animation/AnimTransform.h"
#include "gfx/material/MaterialTable.h"
#include "gfx/mesh/MeshData.h"
#include "gfx/registries/Handles.h"
#include "gfx/renderers/ViewSystem.h"

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
		MeshPtr Mesh;

		MaterialTable Materials;

		FTransform Transform;

		int32_t EntityID = -1;

		std::vector<FTransform> InstanceTransforms;

		std::vector<AnimTransform> BoneTransforms;
	};

	struct FMeshSubmissionContext
	{
		VertexArrayPtr VAO;

		FTransform Transform;

		MaterialTable Materials;

		int32_t EntityID = -1;

		std::vector<glm::mat4> InstanceTransforms;

		uint32_t BoneOffset = 0;

		uint32_t BoneCount = 0;

		bool Active = true;

		uint32_t Generation = 0;
	};

	struct FSubMeshSubmission
	{
		FSubMesh SubMesh;

		uint32_t MeshIndex = 0;

		uint32_t DrawCommandIndex = 0;

		MaterialPtr Material;

		// opaque/transparent, cast_shadows
		std::bitset<2> BitFlags;

		ContextHandle Context;
	};

	using SubMeshSubmissions = std::vector<FSubMeshSubmission>;

	struct FShadowCascadedCreateInfo
	{
		glm::vec2 CameraNearFar;
		glm::vec3 CameraPosition;
		glm::vec3 CameraForward;

		glm::vec3 LightDirection;
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

	struct MultiDrawIndirectCommand
	{
		uint32_t indexCount = 0;
		uint32_t instanceCount = 0;
		uint32_t firstIndex = 0;
		int32_t vertexOffset = 0;
		uint32_t firstInstance = 0;
	};

	struct alignas(16) ObjectData
	{
		glm::mat4 ModelMatrix{1.0f};				  // model matrix
		glm::vec4 CenterRadius{0.f, 0.0f, 0.0f, 0.f}; // bounding sphere center.xyz + radius
		uint32_t ID = 0;							  // which mesh this instance belongs to
		uint32_t BoneOffset = 0;					  // bone offset
		glm::vec4 debugColor;
	};

#define MULTI_DRAW_INDIRECT_STRIDE sizeof(MultiDrawIndirectCommand)
#define MAX_OBJECTS 100
#define VISIBILITY_BUFFER_SIZE 16 + 16 * MAX_OBJECTS
#define OBJECT_STRIDE sizeof(ObjectData)
#define OBJECT_BUFFER_SIZE 16 + OBJECT_STRIDE *MAX_OBJECTS
#define DRAWCOMMAND_BUFFER_SIZE MULTI_DRAW_INDIRECT_STRIDE *MAX_OBJECTS
#define MAX_LIGHTS Lights::sMaxLights
#define MAX_BONES 200
#define BONE_BUFFER_SIZE sizeof(glm::mat4) * MAX_BONES *MAX_OBJECTS

} // namespace BHive