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