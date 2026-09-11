#pragma once

#include "core/Core.h"
#include "core/math/Transform.h"
#include "core/math/boundingbox/AABB.h"
#include "core/serialization/Serialization.h"
#include "gfx/BufferLayout.h"

#define MAX_BONE_INFLUENCE 4

namespace BHive
{
	struct FVertex
	{
		glm::vec3 Position{0.f, 0.f, 0.0f};
		glm::vec2 TexCoord{0.f, 0.f};
		glm::vec3 Normal{0.f, 0.f, 1.f};
		glm::vec3 Tangent{0.f, 0.f, 0.f};
		glm::vec3 BiNormal{0.f, 0.0f, .0f};
		glm::vec4 Color{0.f, 0.0f, 0.f, 0.f};
		int BoneIDs[MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};
		float Weights[MAX_BONE_INFLUENCE] = {0.f, 0.f, 0.f, 0.f};

		static BufferLayout Layout()
		{
			return {{EShaderDataType::Float3}, {EShaderDataType::Float2}, {EShaderDataType::Float3}, {EShaderDataType::Float3},
					{EShaderDataType::Float3}, {EShaderDataType::Float4}, {EShaderDataType::Int4},	 {EShaderDataType::Float4}};
		}
	};

	struct FSubMesh
	{
		uint32_t StartVertex = 0;
		uint32_t StartIndex = 0;
		uint32_t IndexCount = 0;
		int32_t MaterialIndex = 0;
		FTransform Transformation;
		AABB Bounds;
	};

	struct FMeshData
	{
		std::vector<FSubMesh> SubMeshes;
		std::vector<FVertex> Vertices;
		std::vector<uint32_t> Indices;
		uint32_t MaterialCount;
		AABB Bounds;
	};

	template <typename A>
	inline void Serialize(A &ar, FVertex &obj)
	{
		ar(obj.Position, obj.TexCoord, obj.Normal, obj.Tangent, obj.BiNormal, obj.Color, MAKE_BINARY(obj.BoneIDs, MAX_BONE_INFLUENCE * sizeof(int)),
		   MAKE_BINARY(obj.Weights, MAX_BONE_INFLUENCE * sizeof(float)));
	}

	template <typename A>
	inline void Serialize(A &ar, FSubMesh &obj)
	{
		ar(obj.StartIndex, obj.StartVertex, obj.IndexCount, obj.Transformation, obj.MaterialIndex, obj.Bounds);
	}

	template <typename A>
	inline void Save(A &ar, const FMeshData &obj)
	{
		ar(obj.Vertices.size());
		ar(obj.Indices.size());
		ar(obj.SubMeshes, MAKE_BINARY(obj.Vertices.data(), obj.Vertices.size() * sizeof(FVertex)), MAKE_BINARY(obj.Indices.data(), obj.Indices.size() * sizeof(uint32_t)));
	}

	template <typename A>
	inline void Load(A &ar, FMeshData &obj)
	{
		size_t num_verts, num_idx;

		ar(num_verts);
		ar(num_idx);

		obj.Vertices.resize(num_verts);
		obj.Indices.resize(num_idx);

		ar(obj.SubMeshes, MAKE_BINARY(const_cast<FVertex *>(obj.Vertices.data()), obj.Vertices.size() * sizeof(FVertex)),
		   MAKE_BINARY(const_cast<uint32_t *>(obj.Indices.data()), obj.Indices.size() * sizeof(uint32_t)));
	}
} // namespace BHive