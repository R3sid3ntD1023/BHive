#include "SkeletalAnimation.h"
#include "Skeleton.h"

namespace BHive
{
	SkeletalAnimation::SkeletalAnimation(
		float duration, float ticksPerSecond, const Frames &frames, Ref<Skeleton> skeleton,
		const glm::mat4 &globalInverseMatrix)
		: mDuration(duration),
		  mTicksPerSecond(ticksPerSecond),
		  mGlobalInverseTransformation(globalInverseMatrix),
		  mFrameData(frames),
		  mSkeleton(skeleton)
	{
	}
	int32_t SkeletalAnimation::GetPositionIndex(const std::string &name, float animationTime)
	{
		auto &keys = mFrameData[name].mPositions;
		for (size_t i = 0; i < keys.size() - 1; i++)
		{
			if (animationTime < keys[i + 1].mTimeStamp)
				return (int32_t)i;
		}

		ASSERT(0);
		return -1;
	}

	int32_t SkeletalAnimation::GetRotationIndex(const std::string &name, float animationTime)
	{
		auto &keys = mFrameData[name].mRotations;
		for (size_t i = 0; i < keys.size() - 1; i++)
		{
			if (animationTime < keys[i + 1].mTimeStamp)
				return (int32_t)i;
		}

		ASSERT(0);
		return -1;
	}

	int32_t SkeletalAnimation::GetScaleIndex(const std::string &name, float animationTime)
	{
		auto &keys = mFrameData[name].mScales;
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

	glm::vec3 SkeletalAnimation::InterpolatePosition(const std::string &name, float animationTime)
	{

		auto &keys = mFrameData[name].mPositions;
		if (keys.size() == 1)
			return keys[0].mValue;

		int p0 = GetPositionIndex(name, animationTime);
		int p1 = p0 + 1;

		float factor = GetScaleFactor(keys[p0].mTimeStamp, keys[p1].mTimeStamp, animationTime);
		auto position = glm::mix(keys[p0].mValue, keys[p1].mValue, factor);
		return position;
	}

	glm::quat SkeletalAnimation::InterpolateRotation(const std::string &name, float animationTime)
	{
		auto &keys = mFrameData[name].mRotations;
		if (keys.size() == 1)
			return glm::normalize(keys[0].mValue);

		int p0 = GetRotationIndex(name, animationTime);
		int p1 = p0 + 1;

		float factor = GetScaleFactor(keys[p0].mTimeStamp, keys[p1].mTimeStamp, animationTime);
		auto rotation = glm::slerp(keys[p0].mValue, keys[p1].mValue, factor);
		return glm::normalize(rotation);
	}

	glm::vec3 SkeletalAnimation::InterpolateScaling(const std::string &name, float animationTime)
	{
		auto &keys = mFrameData[name].mScales;
		if (keys.size() == 1)
			return keys[0].mValue;

		int p0 = GetScaleIndex(name, animationTime);
		int p1 = p0 + 1;

		float factor = GetScaleFactor(keys[p0].mTimeStamp, keys[p1].mTimeStamp, animationTime);
		auto scale = glm::mix(keys[p0].mValue, keys[p1].mValue, factor);
		return scale;
	}

	void SkeletalAnimation::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		TAssetHandle<Skeleton> handle = mSkeleton;
		ar(mDuration, mTicksPerSecond, mGlobalInverseTransformation, mFrameData, handle);
	}

	void SkeletalAnimation::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);

		TAssetHandle<Skeleton> handle;
		ar(mDuration, mTicksPerSecond, mGlobalInverseTransformation, mFrameData, handle);

		mSkeleton = handle.get();
	}

	bool SkeletalAnimation::Contains(const std::string &name) const
	{
		return mFrameData.contains(name);
	}

	float SkeletalAnimation::CalculateAnimationTimeTicks(float time)
	{
		return fmod(time, mDuration);
	}

	REFLECT(SkeletalAnimation)
	{
		BEGIN_REFLECT(SkeletalAnimation)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY_READ_ONLY("Frames", mFrameData)
		REFLECT_PROPERTY_READ_ONLY("TicksPerSecond", mTicksPerSecond)
		REFLECT_PROPERTY_READ_ONLY("Duration", mDuration);
	}
} // namespace BHive