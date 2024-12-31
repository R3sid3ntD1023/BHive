#pragma once

#include "core/Core.h"
#include "MeshData.h"
#include "Skeleton.h"
#include "AnimationFrames.h"

namespace BHive
{
    struct FImportedAnimationData
    {
        std::string mName;
        float mDuration;
        float TicksPerSecond;
        std::map<std::string, FrameData> mFrames;
        glm::mat4 mGlobalInverseMatrix;
    };

    struct FTextureData
    {
        std::filesystem::path mPath = "";
        size_t mEmbeddedDataSize = 0;
        uint8_t *mEmbeddedData;

        bool is_embedded() const { return mEmbeddedDataSize != 0; }
    };

    struct FMaterialData
    {
        std::string mName;

        // properties
        glm::vec4 mAlbedo = {.5f, .5f, .5f, 1.f};
        float mMetallic = 0.f;
        float mRoughness = 1.0f;

        // textures
        std::unordered_set<std::string> mLoadedTextureNames;
        std::vector<FTextureData> mTextureData;
    };

    struct FMeshImportData
    {
        FMeshData mMeshData;

        // SkeletalMesh

        Bones mBoneData;
        SkeletalNode mSkeletonHeirarchyData;

        // AnimationData
        std::vector<FImportedAnimationData> mAnimationData;

        // materials
        std::vector<FMaterialData> mMaterialData;
    };
}