#include "AnimationClip.h"
#include "SkeletalAnimation.h"
#include "Skeleton.h"
#include "SkeletalPose.h"

namespace BHive
{
	AnimationClip::AnimationClip(const Ref<SkeletalAnimation> &animation)
		: mAnimation(animation)
	{
		auto count = animation->GetSkeleton()->GetBoneCount();
		mBoneTransformations.resize(count, glm::identity<glm::mat4>());
	}

	void AnimationClip::Play(float dt, SkeletalPose &pose)
	{
		mCurrentTime += mAnimation->GetTicksPerSecond() * dt;
		mCurrentTime = mAnimation->CalculateAnimationTimeTicks(mCurrentTime);

		auto &root = mAnimation->GetSkeleton()->GetRoot();
		ReadNodeHeirarchy(root, pose, glm::mat4(1.f), mCurrentTime);
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

	void AnimationClip::SetSkeletalAnimation(const Ref<SkeletalAnimation> &animation)
	{
		mAnimation = animation;
		auto count = animation->GetSkeleton()->GetBoneCount();
		mBoneTransformations.resize(count, glm::mat4(1.f));
	}

	void AnimationClip::ReadNodeHeirarchy(const SkeletalNode &node, SkeletalPose &pose, const glm::mat4 &parent, float time)
	{
		auto skeleton = mAnimation->GetSkeleton();
		auto node_transformation = node.mTransformation;
		auto bone = skeleton->FindBone(node.mName);

		if (mAnimation->Contains(node.mName))
		{
			if (bone)
			{

				auto translation = mAnimation->InterpolatePosition(bone->mName, time);
				auto rotation = mAnimation->InterpolateRotation(bone->mName, time);
				auto scale = mAnimation->InterpolateScaling(bone->mName, time);

				node_transformation = glm::translate(translation) * glm::toMat4(rotation) * glm::scale(scale);
			}
		}

		glm::mat4 global_transformation = parent * node_transformation;

		if (bone)
		{
			pose.SetTransformJointSpace(bone->mID, global_transformation * bone->mOffset);
		}

		for (auto &child : node.mChildren)
		{
			ReadNodeHeirarchy(child, pose, global_transformation, mCurrentTime);
		}
	}

} // namespace BHive
