#pragma once

#include "core/Core.h"
#include "gfx/BufferLayout.h"
#include "math/AABB.hpp"
#include "core/serialization/Serialization.h"

#define MAX_BONE_INFLUENCE 4

namespace BHive
{
	struct FVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 BiNormal;
		glm::vec4 Color;
		int32_t mBoneID[MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};
		float mWeights[MAX_BONE_INFLUENCE] = {0, 0, 0, 0};
		// glm::mat4 Matrix = {1.f};

		static BufferLayout Layout()
		{
			return {{EShaderDataType::Float3}, {EShaderDataType::Float2}, {EShaderDataType::Float3},
					{EShaderDataType::Float3}, {EShaderDataType::Float3}, {EShaderDataType::Float4},
					{EShaderDataType::Int4},   {EShaderDataType::Float4} /*, {EShaderDataType::Mat4}*/};
		}
	};

	struct FSubMesh
	{
		uint32_t StartVertex = 0;
		uint32_t StartIndex = 0;
		uint32_t IndexCount = 0;
		int32_t MaterialIndex = 0;
		glm::mat4 Transformation{1.f};
	};

	struct BHIVE FMeshData
	{
		std::vector<FSubMesh> mSubMeshes;
		std::vector<FVertex> mVertices;
		std::vector<uint32_t> mIndices;
		AABB mBoundingBox;
	};

	template <typename A>
	inline void Serialize(A &ar, FVertex &obj)
	{
		ar(obj.Position, obj.TexCoord, obj.Normal, obj.Tangent, obj.BiNormal, obj.Color,
		   MAKE_BINARY(obj.mBoneID, 4 * sizeof(float)), MAKE_BINARY(obj.mWeights, 4 * sizeof(float)));
	}

	template <typename A>
	inline void Serialize(A &ar, FSubMesh &obj)
	{
		ar(obj.StartIndex, obj.StartVertex, obj.IndexCount, obj.Transformation, obj.MaterialIndex);
	}

	template <typename A>
	inline void Save(A &ar, const FMeshData &obj)
	{
		ar(obj.mVertices.size());
		ar(obj.mIndices.size());
		ar(obj.mSubMeshes, obj.mBoundingBox, MAKE_BINARY(obj.mVertices.data(), obj.mVertices.size() * sizeof(FVertex)),
		   MAKE_BINARY(obj.mIndices.data(), obj.mIndices.size() * sizeof(uint32_t)));
	}

	template <typename A>
	inline void Load(A &ar, FMeshData &obj)
	{
		size_t num_verts, num_idx;

		ar(num_verts);
		ar(num_idx);

		obj.mVertices.resize(num_verts);
		obj.mIndices.resize(num_idx);

		ar(obj.mSubMeshes, obj.mBoundingBox,
		   MAKE_BINARY(const_cast<FVertex *>(obj.mVertices.data()), obj.mVertices.size() * sizeof(FVertex)),
		   MAKE_BINARY(const_cast<uint32_t *>(obj.mIndices.data()), obj.mIndices.size() * sizeof(uint32_t)));
	}
} // namespace BHive