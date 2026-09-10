#include "AnimationClip.h"
#include "SkeletalAnimation.h"
#include "SkeletalPose.h"
#include "Skeleton.h"

namespace BHive
{
	AnimationClip::AnimationClip(SkeletalAnimationPtr animation)
		: mAnimation(animation.As<SkeletalAnimation>())
	{
	}

	void AnimationClip::Play(float dt, SkeletalPose &pose, Skeleton *skeleton)
	{
		mCurrentTime += mAnimation->GetTicksPerSecond() * dt;
		mCurrentTime = mAnimation->CalculateAnimationTimeTicks(mCurrentTime);

		ReadNodeHeirarchy(skeleton, skeleton->GetRoot(), pose, glm::mat4(1.f), mCurrentTime);
	}

	void AnimationClip::PlayFromStart()
	{
		mCurrentTime = 0.f;
	}

	float AnimationClip::GetDuration() const
	{
		return mAnimation->GetDuration();
	}

	float AnimationClip::GetLengthInSeconds() const
	{
		return mAnimation->GetLengthInSeconds();
	}

	void AnimationClip::SetSkeletalAnimation(SkeletalAnimationPtr animation)
	{
		mAnimation = animation.As<SkeletalAnimation>();
	}

	void AnimationClip::ReadNodeHeirarchy(Skeleton *skeleton, const SkeletalNode &node, SkeletalPose &pose, const glm::mat4 &parent, float time)
	{
		auto t = node.Transformation;
		auto bone = skeleton->FindBone(node.NameHash);

		if (mAnimation->Contains(node.NameHash))
		{
			if (bone)
			{
				auto rotation = mAnimation->InterpolateRotation(bone->NameHash, time);
				auto translation = mAnimation->InterpolatePosition(bone->NameHash, time);
				auto scale = mAnimation->InterpolateScaling(bone->NameHash, time);

				t = glm::translate(translation) * glm::toMat4(rotation) * glm::scale(scale);
			}
		}

		glm::mat4 global_transformation = parent * t;

		if (bone)
		{
			auto final = global_transformation * bone->Offset;
			pose.SetTransformJointSpace(bone->ID, final);
		}

		for (auto &child : node.Children)
		{
			ReadNodeHeirarchy(skeleton, child, pose, global_transformation, mCurrentTime);
		}
	}

} // namespace BHive
