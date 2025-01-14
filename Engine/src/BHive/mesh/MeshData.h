#pragma once

#include "core/Core.h"
#include "gfx/BufferLayout.h"
#include "math/AABB.hpp"

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

        static BufferLayout Layout()
        {
            return {
                {EShaderDataType::Float3},
                {EShaderDataType::Float2},
                {EShaderDataType::Float3},
                {EShaderDataType::Float3},
                {EShaderDataType::Float3},
                {EShaderDataType::Float4},
                {EShaderDataType::Int4},
                {EShaderDataType::Float4}};
        }
    };

    struct FSubMesh
    {
        int32_t mMaterialIndex = 0;
        uint32_t mStartVertex = 0;
        uint32_t mStartIndex = 0;
        uint32_t mVertexCount = 0;
        uint32_t mIndexCount = 0;
        uint32_t mInstanceCount = 0;
    };

    struct BHIVE FMeshData
    {
        std::vector<FSubMesh> mSubMeshes;
        std::vector<FVertex> mVertices;
        std::vector<uint32_t> mIndices;
        AABB mBoundingBox;
    };

    inline void Serialize(StreamWriter &ar, const FVertex &obj)
    {
        ar(obj.Position, obj.TexCoord, obj.Normal, obj.Tangent, obj.BiNormal, obj.Color);
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
            ar(obj.mBoneID[i], obj.mWeights[i]);
    }

    inline void Deserialize(StreamReader &ar, FVertex &obj)
    {
        ar(obj.Position, obj.TexCoord, obj.Normal, obj.Tangent, obj.BiNormal, obj.Color);
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
            ar(obj.mBoneID[i], obj.mWeights[i]);
    }

    inline void Serialize(StreamWriter &ar, const FSubMesh &obj)
    {
        ar(obj.mMaterialIndex, obj.mStartVertex, obj.mStartIndex, obj.mVertexCount, obj.mIndexCount, obj.mInstanceCount);
    }

    inline void Deserialize(StreamReader &ar, FSubMesh &obj)
    {
        ar(obj.mMaterialIndex, obj.mStartVertex, obj.mStartIndex, obj.mVertexCount, obj.mIndexCount, obj.mInstanceCount);
    }

    inline void Serialize(StreamWriter &ar, const FMeshData &obj)
    {
        ar(obj.mSubMeshes, obj.mVertices, obj.mIndices, obj.mBoundingBox);
    }

    inline void Deserialize(StreamReader &ar, FMeshData &obj)
    {
        ar(obj.mSubMeshes, obj.mVertices, obj.mIndices, obj.mBoundingBox);
    }
}