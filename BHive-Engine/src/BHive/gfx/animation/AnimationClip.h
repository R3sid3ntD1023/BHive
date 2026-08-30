#pragma once

#include "core/Core.h"

namespace BHive
{
	class Skeleton;
	class SkeletalAnimation;
	struct SkeletalNode;
	class SkeletalPose;

	class BHIVE_API AnimationClip
	{
	public:
		AnimationClip(const Ref<SkeletalAnimation> &animation);

		void Play(float dt, SkeletalPose &pose, Skeleton *skeleton);

		void PlayFromStart();

		float GetDuration() const;

		float GetLengthInSeconds() const;

		void SetSkeletalAnimation(const Ref<SkeletalAnimation> &animation);

	private:
		void ReadNodeHeirarchy(Skeleton *skeleton, const SkeletalNode &node, SkeletalPose &pose, const glm::mat4 &parent, float time);

	private:
		Ref<SkeletalAnimation> mAnimation;
		float mCurrentTime = 0.0f;
	};
} // namespace BHive