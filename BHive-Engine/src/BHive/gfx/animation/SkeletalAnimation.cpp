#include "SkeletalAnimation.h"
#include "Skeleton.h"

namespace BHive
{
	SkeletalAnimation::SkeletalAnimation(float duration, float ticksPerSecond, const Frames &frames)
		: mDuration(duration),
		  mTicksPerSecond(ticksPerSecond),
		  mFrames(frames)
	{
	}
	int32_t SkeletalAnimation::GetPositionIndex(uint64_t hash, float animationTime)
	{
		auto &keys = mFrames[hash].mPositions;
		for (size_t i = 0; i < keys.size() - 1; i++)
		{
			if (animationTime < keys[i + 1].mTimeStamp)
				return (int32_t)i;
		}

		ASSERT(0);
		return -1;
	}

	int32_t SkeletalAnimation::GetRotationIndex(uint64_t hash, float animationTime)
	{
		auto &keys = mFrames[hash].mRotations;
		for (size_t i = 0; i < keys.size() - 1; i++)
		{
			if (animationTime < keys[i + 1].mTimeStamp)
				return (int32_t)i;
		}

		ASSERT(0);
		return -1;
	}

	int32_t SkeletalAnimation::GetScaleIndex(uint64_t hash, float animationTime)
	{
		auto &keys = mFrames[hash].mScales;
		for (size_t i = 0; i < keys.size() - 1; i++)
		{
			if (animationTime < keys[i + 1].mTimeStamp)
				return (int32_t)i;
		}

		ASSERT(0);
		return -1;
	}

	float SkeletalAnimation::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float factor = 0.0f;
		float mid_way_length = animationTime - lastTimeStamp;
		float frame_diff = nextTimeStamp - lastTimeStamp;
		factor = mid_way_length / frame_diff;
		return factor;
	}

	glm::vec3 SkeletalAnimation::InterpolatePosition(uint64_t hash, float animationTime)
	{

		auto &keys = mFrames[hash].mPositions;
		if (keys.size() == 1)
			return keys[0].mValue;

		int p0 = GetPositionIndex(hash, animationTime);
		int p1 = p0 + 1;

		float factor = GetScaleFactor(keys[p0].mTimeStamp, keys[p1].mTimeStamp, animationTime);
		auto position = glm::mix(keys[p0].mValue, keys[p1].mValue, factor);
		return position;
	}

	glm::quat SkeletalAnimation::InterpolateRotation(uint64_t hash, float animationTime)
	{
		auto &keys = mFrames[hash].mRotations;
		if (keys.size() == 1)
			return glm::normalize(keys[0].mValue);

		int p0 = GetRotationIndex(hash, animationTime);
		int p1 = p0 + 1;

		float factor = GetScaleFactor(keys[p0].mTimeStamp, keys[p1].mTimeStamp, animationTime);
		auto rotation = glm::slerp(keys[p0].mValue, keys[p1].mValue, factor);
		return glm::normalize(rotation);
	}

	glm::vec3 SkeletalAnimation::InterpolateScaling(uint64_t hash, float animationTime)
	{
		auto &keys = mFrames[hash].mScales;
		if (keys.size() == 1)
			return keys[0].mValue;

		int p0 = GetScaleIndex(hash, animationTime);
		int p1 = p0 + 1;

		float factor = GetScaleFactor(keys[p0].mTimeStamp, keys[p1].mTimeStamp, animationTime);
		auto scale = glm::mix(keys[p0].mValue, keys[p1].mValue, factor);
		return scale;
	}

	const FrameData *SkeletalAnimation::FindFrames(uint64_t hash) const
	{
		if (mFrames.contains(hash))
			return &mFrames.at(hash);

		return nullptr;
	}

	bool SkeletalAnimation::Contains(uint64_t hash) const
	{
		return mFrames.contains(hash);
	}

	float SkeletalAnimation::CalculateAnimationTimeTicks(float time)
	{
		return fmod(time, mDuration);
	}

	REFLECT(SkeletalAnimation)
	{
		BEGIN_REFLECT(SkeletalAnimation)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY_READ_ONLY("Frames", mFrames)
		REFLECT_PROPERTY_READ_ONLY("TicksPerSecond", mTicksPerSecond)
		REFLECT_PROPERTY_READ_ONLY("Duration", mDuration);
	}
} // namespace BHive