#pragma once

#include "core/Core.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	class Skeleton;
	struct SkeletalNode;
	class SkeletalPose;
	class SkeletalAnimation;

	class BHIVE_API AnimationClip
	{
	public:
		AnimationClip(SkeletalAnimationPtr animation);

		void Play(float dt, SkeletalPose &pose, Skeleton *skeleton);

		void PlayFromStart();

		float GetDuration() const;

		float GetLengthInSeconds() const;

		void SetSkeletalAnimation(SkeletalAnimationPtr animation);

	private:
		void ReadNodeHeirarchy(Skeleton *skeleton, const SkeletalNode &node, SkeletalPose &pose, const glm::mat4 &parent, float time);

	private:
		SkeletalAnimation *mAnimation;
		float mCurrentTime = 0.0f;
	};
} // namespace BHive