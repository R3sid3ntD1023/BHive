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

   
}