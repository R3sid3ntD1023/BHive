#pragma once

#include "core/Core.h"

namespace BHive
{
	class SkeletalAnimation;
	struct SkeletalNode;
	class SkeletalPose;

	class AnimationClip
	{
	public:
		AnimationClip(const Ref<SkeletalAnimation> &animation);

		void Play(float dt, SkeletalPose &pose);

		void PlayFromStart();

		float GetDuration() const;

		float GetLengthInSeconds() const;

		void SetSkeletalAnimation(const Ref<SkeletalAnimation> &animation);

		const std::vector<glm::mat4> &GetBoneTransformations() const { return mBoneTransformations; }

	private:
		void ReadNodeHeirarchy(const SkeletalNode &node, SkeletalPose &pose, const glm::mat4 &parent, float time);

	private:
		Ref<SkeletalAnimation> mAnimation;
		std::vector<glm::mat4> mBoneTransformations;
		float mCurrentTime = 0.0f;
	};
} // namespace BHive