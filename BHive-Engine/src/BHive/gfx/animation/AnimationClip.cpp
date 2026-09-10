#include "AnimationClip.h"
#include "SkeletalAnimation.h"
#include "SkeletalPose.h"
#include "Skeleton.h"
#include "core/debug/Instrumentor.h"

namespace BHive
{
	AnimationClip::AnimationClip(SkeletalAnimationPtr animation, SkeletonPtr skeleton)
	{
		ASSERT(skeleton && animation);

		mAnimation = animation.As<SkeletalAnimation>();
		mSkeleton = skeleton.As<Skeleton>();
		BuildRuntime(mSkeleton);
		mCurrentTime = 0.0f;
	}

	void AnimationClip::Play(float dt, SkeletalPose &pose)
	{
		ASSERT(mSkeleton && mAnimation);

		float previous = mCurrentTime;

		mCurrentTime += mAnimation->GetTicksPerSecond() * dt;
		mCurrentTime = mAnimation->CalculateAnimationTimeTicks(mCurrentTime);

		if (mCurrentTime < previous)
		{
			for (auto &bone : mRuntimeBones)
			{
				bone.PositonKey = 0;
				bone.RotationKey = 0;
				bone.ScaleKey = 0;
			}
		}

		// evaluate local transforms
		{
			BH_PROFILE_SCOPE("Evaluate");

			for (uint32_t i = 0; i < mRuntimeBones.size(); i++)
			{
				auto &bone = mRuntimeBones[i];

				mLocalTransforms[i] = Evaluate(bone, mCurrentTime);
			}
		}

		// build globals
		{
			BH_PROFILE_SCOPE("Bind Globals");

			for (uint32_t i = 0; i < mRuntimeBones.size(); i++)
			{
				auto &bone = mRuntimeBones[i];

				auto &local = mLocalTransforms[i];
				glm::mat4 t = glm::translate(local.Position) * glm::toMat4(local.Rotation) * glm::scale(local.Scale);
				if (bone.Parent == INVALID_PARENT)
				{
					mGlobalTransforms[i] = t;
				}
				else
				{
					mGlobalTransforms[i] = mGlobalTransforms[bone.Parent] * t;
				}
			}
		}

		// final pose
		{
			BH_PROFILE_SCOPE("Final Pose");

			for (uint32_t i = 0; i < mRuntimeBones.size(); i++)
			{
				auto &bone = mRuntimeBones[i];

				if (!bone.Bone)
					continue;

				auto final = mGlobalTransforms[i] * bone.Bone->Offset;
				pose.SetTransformJointSpace(bone.Bone->ID, final);
			}
		}
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

	void AnimationClip::BuildRuntime(Skeleton *skeleton)
	{

		mRuntimeBones.clear();
		mLocalTransforms.clear();
		mGlobalTransforms.clear();

		BuildNode(skeleton, skeleton->GetRoot(), INVALID_PARENT);
	}

	void AnimationClip::BuildNode(Skeleton *skeleton, const SkeletalNode &node, uint16_t parent)
	{
		RuntimeBone runtime;

		runtime.Parent = parent;
		runtime.Bone = skeleton->FindBone(node.NameHash);
		runtime.Frames = mAnimation->FindFrames(node.NameHash);

		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(node.Transformation, runtime.BindPose.Scale, runtime.BindPose.Rotation, runtime.BindPose.Position, skew, perspective);

		uint16_t parentIndex = (uint16_t)mRuntimeBones.size();

		mRuntimeBones.push_back(runtime);

		mLocalTransforms.emplace_back();
		mGlobalTransforms.emplace_back(1.0f);

		for (const auto &child : node.Children)
			BuildNode(skeleton, child, parentIndex);
	}

	float AnimationClip::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float factor = 0.0f;
		float mid_way_length = animationTime - lastTimeStamp;
		float frame_diff = nextTimeStamp - lastTimeStamp;
		factor = mid_way_length / frame_diff;
		return factor;
	}

	LocalPose AnimationClip::Evaluate(RuntimeBone &bone, float time)
	{
		if (!bone.Frames)
			return bone.BindPose;

		return {InterpolatePosition(bone, time), InterpolateRotation(bone, time), InterpolateScaling(bone, time)};
	}

	glm::vec3 AnimationClip::InterpolatePosition(RuntimeBone &bone, float time)
	{
		auto &keys = bone.Frames->mPositions;
		uint16_t &current = bone.PositonKey;

		if (keys.empty())
			return bone.BindPose.Position;

		if (keys.size() == 1)
			return keys[0].mValue;

		while (current + 1 < keys.size() && time >= keys[current + 1].mTimeStamp)
		{
			++current;
		}

		uint32_t p0 = current;
		uint32_t p1 = std::min(current + 1, (uint16_t)(keys.size()) - 1);

		float factor = GetScaleFactor(keys[p0].mTimeStamp, keys[p1].mTimeStamp, time);
		return glm::mix(keys[p0].mValue, keys[p1].mValue, factor);
	}

	glm::quat AnimationClip::InterpolateRotation(RuntimeBone &bone, float time)
	{
		auto &keys = bone.Frames->mRotations;
		uint16_t &current = bone.RotationKey;

		if (keys.empty())
			return bone.BindPose.Rotation;

		if (keys.size() == 1)
			return keys[0].mValue;

		while (current + 1 < keys.size() && time >= keys[current + 1].mTimeStamp)
		{
			++current;
		}

		uint32_t p0 = current;
		uint32_t p1 = std::min(current + 1, (uint16_t)(keys.size()) - 1);

		float factor = GetScaleFactor(keys[p0].mTimeStamp, keys[p1].mTimeStamp, time);
		return glm::normalize(glm::slerp(keys[p0].mValue, keys[p1].mValue, factor));
	}

	glm::vec3 AnimationClip::InterpolateScaling(RuntimeBone &bone, float time)
	{
		auto &keys = bone.Frames->mScales;
		uint16_t &current = bone.ScaleKey;

		if (keys.empty())
			return bone.BindPose.Scale;

		if (keys.size() == 1)
			return keys[0].mValue;

		while (current + 1 < keys.size() && time >= keys[current + 1].mTimeStamp)
		{
			++current;
		}

		uint32_t p0 = current;
		uint32_t p1 = std::min(current + 1, (uint16_t)(keys.size()) - 1);

		float factor = GetScaleFactor(keys[p0].mTimeStamp, keys[p1].mTimeStamp, time);
		return glm::mix(keys[p0].mValue, keys[p1].mValue, factor);
	}

} // namespace BHive
