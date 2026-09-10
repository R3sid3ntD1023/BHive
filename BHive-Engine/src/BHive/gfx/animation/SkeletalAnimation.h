#pragma once

#include "AnimationFrames.h"
#include "asset/Asset.h"
#include "core/Core.h"

namespace BHive
{
	class Skeleton;

	class BHIVE_API SkeletalAnimation : public Asset
	{
	public:
		using Frames = std::unordered_map<uint64_t, FrameData>;

		SkeletalAnimation() = default;
		SkeletalAnimation(float duration, float ticksPerSecond, const Frames &frames);

		bool Contains(uint64_t hash) const;

		float GetTicksPerSecond() const { return mTicksPerSecond; }
		float GetDuration() const { return mDuration; }
		float GetLengthInSeconds() const { return mDuration / mTicksPerSecond; }

		float CalculateAnimationTimeTicks(float time);

		const FrameData *FindFrames(uint64_t hash) const;

		REFLECTABLEV(Asset)
	private:
		float mDuration{};
		float mTicksPerSecond{};
		Frames mFrames;
	};

	REFLECT_EXTERN(SkeletalAnimation)
} // namespace BHive