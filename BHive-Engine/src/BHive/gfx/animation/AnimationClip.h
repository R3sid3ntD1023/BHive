#pragma once

#include "AnimationFrames.h"
#include "Bone.h"
#include "asset/AssetHandle.h"
#include "core/Core.h"

namespace BHive
{
	class Skeleton;
	struct SkeletalNode;
	class SkeletalPose;
	class SkeletalAnimation;

	struct RuntimeBone
	{
		uint16_t Parent;
		const Bone *Bone;
		const FrameData *Frames;

		AnimTransform BindPose;

		uint16_t PositonKey = 0;
		uint16_t RotationKey = 0;
		uint16_t ScaleKey = 0;
	};

	class BHIVE_API AnimationClip
	{

	public:
		static constexpr uint16_t INVALID_PARENT = UINT16_MAX;

		AnimationClip(AssetHandle<SkeletalAnimation> animation, AssetHandle<Skeleton> skeleton);

		void Play(float dt, SkeletalPose &pose);

		void PlayFromStart();

		float GetDuration() const;

		float GetLengthInSeconds() const;

	private:
		void BuildRuntime(Skeleton *skeleton);

		void BuildNode(Skeleton *skeleton, const SkeletalNode &node, uint16_t parent);

		glm::vec3 InterpolatePosition(RuntimeBone &bone, float time);

		glm::quat InterpolateRotation(RuntimeBone &bone, float time);

		glm::vec3 InterpolateScaling(RuntimeBone &bone, float time);

		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

		AnimTransform Evaluate(RuntimeBone &bone, float time);

	private:
		AssetHandle<SkeletalAnimation> mAnimation;
		AssetHandle<Skeleton> mSkeleton;

		float mCurrentTime = 0.0f;
		std::vector<RuntimeBone> mRuntimeBones;
		std::vector<AnimTransform> mLocalTransforms;
		std::vector<AnimTransform> mGlobalTransforms;
	};
} // namespace BHive