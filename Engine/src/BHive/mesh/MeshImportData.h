#pragma once

#include "core/Core.h"
#include "MeshData.h"
#include "Skeleton.h"
#include "AnimationFrames.h"

namespace BHive
{
	struct FImportedAnimationData
	{
		std::string mName{""};
		float mDuration{0.f};
		float TicksPerSecond{0.f};
		glm::mat4 mGlobalInverseMatrix{1.0f};
		std::map<std::string, FrameData> mFrames;
	};

	struct FTextureData
	{
		std::string mType{""};
		std::filesystem::path mPath{""};
		Buffer mEmbeddedData;

		bool is_embedded() const { return mEmbeddedData; }
	};

	struct FMaterialData
	{
		std::string mName;
		glm::vec4 mAlbedo = {.5f, .5f, .5f, 1.f};
		float mMetallic = 0.f;
		float mRoughness = 1.0f;
		std::unordered_set<std::string> mLoadedTextureNames;
		std::vector<FTextureData> mTextureData;
	};

	struct FMeshImportData
	{
		FMeshData mMeshData;
		Bones mBoneData;
		SkeletalNode mSkeletonHeirarchyData;
		std::vector<FImportedAnimationData> mAnimationData;
		std::vector<FMaterialData> mMaterialData;
	};
} // namespace BHive